#include "tcp_server.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#include "logger.h"
#include "config.h"
#include "tool_functions.h"

using namespace LOG_PWER;

extern int errno;

extern config *g_config;

const string k_back_log_size_key = "back_log_size";
const string k_thread_num_key = "thread_num";
const string k_queue_size_key = "queue_size";
const string k_sockets_size_key = "sockets_size";

const int k_accept_thread_num = 5;

const int k_epoll_time_out = 5000;

TcpServer::TcpServer(const string &ip, unsigned short port, int split_size):
server_ip_(ip),
server_port_(port),
socket_fd_(-1),
started_(false),
runing_(true),
server_task_(NULL),
split_size_(split_size)
{
	LOG_POWER_LOG_TRACE("tcp server constructor...");
}


TcpServer::~TcpServer()
{
	if (socket_fd_ != -1)
	{
		close(socket_fd_);
	}
	if (server_task_)
	{
		delete server_task_;
	}
}

ErrorValue TcpServer::LoadConfig()
{
	if (!g_config->GetIntValue(k_back_log_size_key, back_log_size_))
	{
		LOG_POWER_LOG_INFO("get back log size failed, use default value:2048");
		back_log_size_ = 2048;
	}

	if (!g_config->GetIntValue(k_thread_num_key, thread_num_size_))
	{
		LOG_POWER_LOG_INFO("get thread num failed, use default value:500");
		thread_num_size_ = 500;
	}
	if (!g_config->GetIntValue(k_queue_size_key, queue_size_))
	{
		LOG_POWER_LOG_INFO("get queue size failed, use default value:300");
		queue_size_ = 300;

	}
	if (!g_config->GetIntValue(k_sockets_size_key, max_clients_size_))
	{
		LOG_POWER_LOG_INFO("get queue size failed, use default value:300");
		max_clients_size_ = 300;

	}

	return kOk;
}


ErrorValue TcpServer::DeleEventFromEpoll(int epoll_fd, ProcessTask *task)
{
	if (task == NULL || task->GetSocketFd() < 0 || task->GetSocketFd() > 65535 || epoll_fd < 0 || epoll_fd > 65535)
	{
		LOG_POWER_LOG_FATAL("DelEventFromEpoll:invalid paramter");
		return kInvalidParams;
	}
	if (!task->if_in_epoll_)
	{
		return kOk;
	}
	epoll_event event;
	//采用了ET，提供效率
	event.events = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLET;
	event.data.ptr = task;
	int result = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, task->GetSocketFd(), &event);
	if (result != 0)
	{
		LOG_POWER_LOG_FATAL("delete socket %d from epoll:%d ctl failed:%s", task->GetSocketFd(), epoll_fd, strerror(errno));
		return kDelFromEpollFailed;
	}
	task->if_in_epoll_ = false;
	return kOk;
}

ErrorValue TcpServer::AddReadEventToEpoll(int epoll_fd, ProcessTask *task)
{
	if (task == NULL || task->GetSocketFd() < 0 || task->GetSocketFd() > 65535 || epoll_fd < 0 || epoll_fd > 65535)
	{
		LOG_POWER_LOG_FATAL("AddReadEventToEpoll:invalid paramter");
		return kInvalidParams;
	}
	if (task->if_in_epoll_)
	{
		return kOk;
	}
	epoll_event event;
	//采用了ET，提供效率
	event.events = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLET;
	event.data.ptr = (void*)task;
	int result = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, task->GetSocketFd(), &event);
	if (result != 0)
	{
		LOG_POWER_LOG_FATAL("add socket %d to epoll:%d ctl failed:%s", task->GetSocketFd(), epoll_fd, strerror(errno));
		return kAddToEpollFailed;
	}
	task->if_in_epoll_ = true;
	return kOk;
}

void* TcpServer::AcceptThreadLoop(void *paramter)
{
	TcpServer *server = (TcpServer*)paramter;

	//创建epoll操作相关
	//max_clients_size参数在linux内核超过2.6.8之后就无效了，linux会自动设置要求的数据结构的大小
	int current_max_client_num = server->max_clients_size_ / k_accept_thread_num;
	int epoll_fd = epoll_create(current_max_client_num);
	if (epoll_fd < 0)
	{
		LOG_POWER_LOG_FATAL("crate epoll fd failed:%s", strerror(errno));
		server->runing_ = false;
		return NULL;
	}
	LOG_POWER_LOG_TRACE("create epoll fd successfully:%d", epoll_fd);
	epoll_event *epoll_event_list = new epoll_event[current_max_client_num];
	memset(epoll_event_list, 0, current_max_client_num * sizeof(epoll_event));
	//创建实际的处理函数
	int result = 0;
	bool has_server_socket = false;
	ErrorValue return_value = kOk;
	while (server->runing_)
	{
		//epoll_wait
		result = pthread_mutex_trylock(&(server->server_socket_lock_));
		if (result == 0)
		{
			//获得了锁，则将server socket也加入到epoll监听队列中
			//LOG_POWER_LOG_TRACE("epoll fd get the server socket lock:%d", epoll_fd);
			return_value = AddReadEventToEpoll(epoll_fd, server->server_task_);
			if (return_value != kOk)
			{
				LOG_POWER_LOG_TRACE("add listen socket to epoll fd:%d failed.", epoll_fd);
				pthread_mutex_unlock(&server->server_socket_lock_);
				has_server_socket = false;
			}
			else
				has_server_socket = true;
		}
		else
			has_server_socket = false;


		int count = epoll_wait(epoll_fd, epoll_event_list, current_max_client_num, k_epoll_time_out);
		if (count == -1)
		{
			LOG_POWER_LOG_FATAL("epoll:%d wait return failed:%s", epoll_fd, strerror(errno));
		}
		else if (count > 0)
		{
			LOG_POWER_LOG_TRACE("epoll fd:%d received %d requests", epoll_fd, count);
			for (int index = 0; index != count; index++)
			{
				ProcessTask *task = (ProcessTask*)epoll_event_list[index].data.ptr;
				if (task ==  server->server_task_)
				{
					//监听socket收到了连接，需要进行accept
					while (true)
					{
						sockaddr_in client_addr;
						//虽然是出参，也需要对client_addr_len进行初始化，否则会返回空的addr
						int client_addr_len = sizeof(client_addr);
						int client_socket = accept(task->GetSocketFd(), (sockaddr*)&client_addr, (socklen_t*)&client_addr_len);
						if (client_socket == -1)
						{
							//accept完毕
							LOG_POWER_LOG_TRACE("epoll fd:%d accept finished.", epoll_fd);
							break;
						}
						else
						{
							LOG_POWER_LOG_TRACE("epoll fd:%d accept new client fd:%d", epoll_fd, client_socket);
							if (SetNonBlock(client_socket) != 0)
							{
								LOG_POWER_LOG_FATAL("epoll fd:%d set new client fd:%d to non block failed:%s", epoll_fd, client_socket, strerror(errno));
								close(client_socket);
								continue;
							}
							//为其创建一个新的任务
							ProcessTask *new_task = new ProcessTask(server->split_size_);
							new_task->SetSocketFd(client_socket);
							//在task中增加一些其他的任务信息
							char buf[32] = {0};
							const char *client_ip = inet_ntop(AF_INET, &(client_addr.sin_addr), buf, 32);
							if (client_ip == NULL)
							{
								LOG_POWER_LOG_FATAL("turned ip to string failed:%s", strerror(errno));
								delete new_task;
								continue;
							}
							LOG_POWER_LOG_TRACE("recived ip is:%s", client_ip);
							new_task->SetClientIp(client_ip);
							new_task->SetEpollFd(epoll_fd);

							//accept新的连接
							return_value = AddReadEventToEpoll(epoll_fd, new_task);
							if (return_value != kOk)
							{
								LOG_POWER_LOG_TRACE("add new socket fd:%d to epoll fd:%d failed.", client_socket, epoll_fd);
								delete new_task;
								continue;
							}
						}
					}
				}
				else
				{
					//收到client socket的可读事件，将其加入到queue中
					epoll_event event = epoll_event_list[index];
					if (DeleEventFromEpoll(epoll_fd, task) != kOk)
					{
						//将client_fd从task中删除失败
						LOG_POWER_LOG_FATAL("delete client socket from epoll failed.");
					}
					LOG_POWER_LOG_TRACE("epoll fd:%d received client data:%d", epoll_fd, task->GetSocketFd());
					if (event.events & EPOLLERR || event.events & EPOLLHUP)
					{
						//句柄断开或者出错
						LOG_POWER_LOG_FATAL("epll fd:%d received error or hup event,client socket is:%d", epoll_fd, task->GetSocketFd());
						delete task;
						continue;
					}
					if (PushToQueue(server, task) != kOk)
					{
						//加入到队列中失败
						LOG_POWER_LOG_FATAL("push task to queue failed.");
						delete task;
						continue;
					}
					LOG_POWER_LOG_TRACE("epoll fd:%d pushed client task to queue:%d", epoll_fd, task->GetSocketFd());
				}
			}
		}

		if (has_server_socket)
		{
			//释放对于server socket及锁，给别的线程机会去处理。
			DeleEventFromEpoll(epoll_fd, server->server_task_);
			pthread_mutex_unlock(&(server->server_socket_lock_));
			//LOG_POWER_LOG_TRACE("epoll fd released the server socket lock:%d", epoll_fd);
		}

	}

	LOG_POWER_LOG_INFO("accept loop exit...");
	return NULL;

}

void* TcpServer::ServerLoop(void *paramter)
{
	//线程处理函数
	TcpServer *server = (TcpServer*)paramter;
	while (server->runing_)
	{
		pthread_mutex_lock(&(server->queue_mutex_lock_));
		while (server->task_queue_.empty() && server->runing_)
		{
			//LOG_POWER_LOG_TRACE("task queue is empty.");
			timeval tv;
			timespec outtime;
			gettimeofday(&tv,  NULL);
			outtime.tv_sec = tv.tv_sec + 5;
			outtime.tv_nsec = tv.tv_usec * 1000;
			pthread_cond_timedwait(&(server->queue_cond_lock_), &(server->queue_mutex_lock_), &outtime);
		}
		if (!server->runing_)
		{
			LOG_POWER_LOG_INFO("server stop, return.");
			pthread_mutex_unlock(&(server->queue_mutex_lock_));
			return NULL;
		}
		ProcessTask *task = server->task_queue_.back();
		server->task_queue_.pop();
		pthread_mutex_unlock(&(server->queue_mutex_lock_));
		LOG_POWER_LOG_TRACE("epoll　fd:%d will deal client fd:%d", task->GetEpollFd(), task->GetSocketFd());


		ErrorValue result = task->ProcessReadTask();

		
		if (result < 0)
		{
			delete task;
			continue;
		}
		//eagin，继续读
		else if (result == kReadEagain)
		{
			//将当前的socket加入的epoll队列中，继续等待处理
			if (AddReadEventToEpoll(task->GetEpollFd(), task) != kOk)
			{
				LOG_POWER_LOG_FATAL("add client :%s socket to epoll failed.");
				//加入到epoll中失败，那么导致新的数据无法获取，关闭本连接
				delete task;
				continue;
			}
		}
		else
		{
			//不可能出现
			LOG_POWER_LOG_FATAL("it is impossible here.");
			delete task;
			continue;
		}
		
	}

	LOG_POWER_LOG_INFO("server loop exit...");
	return NULL;

}

ErrorValue TcpServer::StartServer()
{
	if (started_)
	{
		return kOk;
	}
	
	//校验参数，并初始化监听地址
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port_);
	if (inet_pton(AF_INET, server_ip_.c_str(), &(server_addr.sin_addr)) != 1)
	{
		LOG_POWER_LOG_FATAL("turn ip string to number format failed.");
		return kInvalidParams;
	}                                                                                   


	socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd_ == -1)
	{
		LOG_POWER_LOG_FATAL("create server socket failed:%s", strerror(errno));
		return kCreateSocketFailed;
	}
	int value = 1;
	setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
	struct linger li;
	memset(&li, 0, sizeof(li));
	li.l_onoff = 1;
	li.l_linger = 1;
	setsockopt(socket_fd_, SOL_SOCKET, SO_LINGER, (const char*)&li, sizeof(li));
	int result = bind(socket_fd_, (sockaddr*)&server_addr, sizeof(server_addr));
	if (result != 0)
	{
		LOG_POWER_LOG_FATAL("bind failed:%s", strerror(result));
		return kBindFailed;
	}

	result = listen(socket_fd_, back_log_size_);
	if (result != 0)
	{
		LOG_POWER_LOG_FATAL("listen failed:%s", strerror(result));
		return kListenFailed;
	}
	SetNonBlock(socket_fd_);

	server_task_ = new ProcessTask(split_size_);
	server_task_->SetSocketFd(socket_fd_);

	//初始化server socket锁
	result = pthread_mutex_init(&server_socket_lock_, NULL);
	if (result != 0)
	{
		LOG_POWER_LOG_FATAL("init server socket lock failed:%s", strerror(errno));
		return kInitThreadRelatedFailed;
	}

	result = pthread_mutex_init(&queue_mutex_lock_, NULL);
	if (result != 0)
	{
		LOG_POWER_LOG_FATAL("create queue mutex failed.");
		return kInitThreadRelatedFailed;
	}
	result = pthread_cond_init(&queue_cond_lock_, NULL);
	if (result != 0)
	{
		LOG_POWER_LOG_FATAL("create queue cond failed.");
		return kInitThreadRelatedFailed;
	}

	//创建accept线程
	pthread_attr_t attr;
	result = pthread_attr_init(&attr);
	if (result != 0)
	{
		LOG_POWER_LOG_FATAL("init pthread attr failed.");
		return kInitThreadRelatedFailed;
	}
	//不需要join
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	pthread_t thread_id;
	for (int index = 0; index != thread_num_size_; index++)
	{
		result = pthread_create(&thread_id, &attr, ServerLoop, this);
		if (result != 0)
		{
			LOG_POWER_LOG_FATAL("create server loop thread failed:%s", strerror(errno));
			return kInitThreadRelatedFailed;
		}
	}
	
	for (int index = 0; index != k_accept_thread_num; index++)
	{
		result = pthread_create(&thread_id, &attr, AcceptThreadLoop, this);
		if (result != 0)
		{
			LOG_POWER_LOG_FATAL("create accept thread failed:%s", strerror(errno));
			return kInitThreadRelatedFailed;
		}
	}

	started_ = true;
	return kOk;
}


ErrorValue TcpServer::PushToQueue(TcpServer *this_, const ProcessTask *task)
{
	if (this_ == NULL || task == NULL)
	{
		LOG_POWER_LOG_FATAL("PushToQueue:invalid paramter.");
		return kInvalidParams;
	}

	int result = pthread_mutex_lock(&(this_->queue_mutex_lock_));
	if (result != 0)
	{
		LOG_POWER_LOG_FATAL("lock queue mutex failed:%s", strerror(errno));
		return kInitThreadRelatedFailed;
	}
	if ((int)this_->task_queue_.size() >= this_->queue_size_)
	{
		LOG_POWER_LOG_FATAL("queue is full:%lu", this_->task_queue_.size());
		pthread_mutex_unlock(&(this_->queue_mutex_lock_));
		return kQueueIsFull;
	}
	this_->task_queue_.push((ProcessTask*)task);
	result = pthread_cond_signal(&(this_->queue_cond_lock_));
	pthread_mutex_unlock(&(this_->queue_mutex_lock_));
	if (result != 0)
	{
		LOG_POWER_LOG_FATAL("cond singal failed:%s", strerror(errno));
		return kInitThreadRelatedFailed;
	}

	return kOk;
}
