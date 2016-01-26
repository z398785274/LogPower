#ifndef LOG_POWER_TCP_SERVER_H
#define LOG_POWER_TCP_SERVER_H

#include <string>
#include <queue>
#include <sys/epoll.h>
#include <pthread.h>
using namespace std;

#include "err_no.h"
#include "process_task.h"

class TcpServer
{
public:
	TcpServer(const string &ip, unsigned short port, int split_size);
	~TcpServer();
public:
	ErrorValue LoadConfig();
	ErrorValue StartServer();

private:
	//将client socket事件放入到处理队列中
	ErrorValue AddReadEventToQueue();

	ErrorValue CreateTask();
private:
	static void* ServerLoop(void *paramter);
	static void* AcceptThreadLoop(void *paramter);
	//读增加
	static ErrorValue AddReadEventToEpoll(int epoll_fd, ProcessTask *task);

	//写增加

	//从epoll_fd队列中删除监听事件
	static ErrorValue DeleEventFromEpoll(int epoll_fd, ProcessTask *task);

	static ErrorValue PushToQueue(TcpServer *this_, const ProcessTask *task);
private:
	queue<ProcessTask*> task_queue_;
private:
	string server_ip_;
	unsigned short server_port_;
	int socket_fd_;
	int queue_size_;
	int thread_num_size_;
	int max_clients_size_;
	int back_log_size_;
	bool started_;
	bool runing_;

	ProcessTask *server_task_;

	//线程抢占server_socket_fd去accept的锁
	pthread_mutex_t server_socket_lock_;

	//queue访问锁
	pthread_mutex_t queue_mutex_lock_;
	pthread_cond_t queue_cond_lock_;

	int split_size_;

};
#endif
