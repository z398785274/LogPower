#include "tcp_client.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>

#include "logger.h"
#include "tool_functions.h"
#include "power.loginfo.pb.h"

using namespace LOG_PWER;
extern int errno;

TcpClient::TcpClient(const string &ip, unsigned short port):
if_connected_(false),
ip_(ip),
port_(port),
socket_fd_(-1)
{
}

ErrorValue TcpClient::ConnectToServer(int time_out_in_ms)
{
	if (if_connected_)
	{
		return kOk;
	}
	if (socket_fd_ == -1)
	{
		socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
	}
	if (socket_fd_ == -1)
	{
		LOG_POWER_LOG_FATAL("create socket fd failed:%s", strerror(errno));
		return kCreateSocketFailed;
	}

	sockaddr_in target_addr;
	target_addr.sin_family = AF_INET;
	int result = inet_pton(AF_INET, ip_.c_str(), &(target_addr.sin_addr));
	//int result = inet_pton(AF_INET, "127.0.0.1", &(target_addr.sin_addr));
	if (result != 1)
	{
		LOG_POWER_LOG_FATAL("turn ip string to number format failed:%s", strerror(errno));
		ResetClient();
		return kInvalidParams;
	}
	target_addr.sin_port = htons(port_);

	if (SetNonBlock(socket_fd_) != 0)
	{
		LOG_POWER_LOG_FATAL("set non block failed:%s", strerror(errno));
		ResetClient();
		return kSetNonBlockFailed;
	}

	/*
	使用非阻塞 connect 需要注意的问题是：
		1. 很可能 调用 connect 时会立即建立连接（比如，客户端和服务端在同一台机子上），必须处理这种情况。
		2. Posix 定义了两条与 select 和 非阻塞 connect 相关的规定：
		1）连接成功建立时，socket 描述字变为可写。（连接建立时，写缓冲区空闲，所以可写）
		2）连接建立失败时，socket 描述字既可读又可写。 （由于有未决的错误，从而可读又可写）
	*/
	result = connect(socket_fd_, (sockaddr*)&target_addr, sizeof(target_addr));

	if (result == 0)
	{
		//连接成功
		SetBlock(socket_fd_);
		if_connected_ = true;
		return kOk;
	}

	if (result < 0 && errno != EINPROGRESS)
	{
		LOG_POWER_LOG_FATAL("connect to server failed:%s", strerror(errno));
		ResetClient();
		SetBlock(socket_fd_);
		return kConnectToServerFailed;
	}



	fd_set w_set, r_set;
	FD_ZERO(&w_set);
	FD_SET(socket_fd_, &w_set);
	r_set = w_set;
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = time_out_in_ms * 1000;

	result = select(socket_fd_ + 1, &r_set, &w_set, NULL, &tv);
	SetBlock(socket_fd_);
	if (result == 0)
	{
		LOG_POWER_LOG_FATAL("connect select time out.");
		ResetClient();
		return kConnectToServerFailed;
	}
	
	int err;
	socklen_t len = sizeof(err);
	if (FD_ISSET(socket_fd_, &r_set))
	{
		//可读，建立连接失败
		if (getsockopt(socket_fd_, SOL_SOCKET, SO_ERROR, &err, &len) < 0)
		{
			LOG_POWER_LOG_FATAL("get socket opt failed:%s", strerror(errno));
			ResetClient();
			return kConnectToServerFailed;
		}
		ResetClient();
		LOG_POWER_LOG_FATAL("connect to server failed:%s", strerror(err));
		return kConnectToServerFailed;
	}

	if (FD_ISSET(socket_fd_, &w_set))
	{
		//可写，真正建立了连接
		if_connected_ = true;
		return kOk;
	}
	//不可写，建立连接失败
	if (getsockopt(socket_fd_, SOL_SOCKET, SO_ERROR, &err, &len) < 0)
	{
		LOG_POWER_LOG_FATAL("get socket opt failed:%s", strerror(errno));
		ResetClient();
		return kConnectToServerFailed;
	}
	ResetClient();
	LOG_POWER_LOG_FATAL("connect to server failed:%s", strerror(err));
	return kConnectToServerFailed;
	
}


ErrorValue TcpClient::SendToServer(const char* log_name, const char* buf, int buf_len)
{
	if (if_connected_)
	{
		int send_len = 0;
		//对所发的块进行序列化
		power::loginfo info;
		info.set_log_name(log_name);
		info.set_log_content(buf);
		info.set_log_len(buf_len);
		int current_len = info.ByteSize();
		char* send_buf = new char[current_len + 8];
		static int32_t border_num = htonl(323297739);
		memcpy(send_buf, &border_num, 4);
		int32_t body_len = htonl(current_len);
		memcpy(send_buf + 4, &body_len, 4);
		if (!info.SerializeToArray(send_buf + 8, current_len))
		{
			LOG_POWER_LOG_FATAL("Serialize to array failed.");
			delete[] send_buf;
			return kSerailizeFailed;
		}
		int total_len = current_len + 8;
		while (true)
		{
			int len = send(socket_fd_, send_buf + send_len, total_len - send_len, 0);
			if (len == -1)
			{
				LOG_POWER_LOG_FATAL("tcp client send buf to server failed:%s", strerror(errno));
				delete[] send_buf;
				return kSendDataToServerFailed;
			}
			send_len += len;
			if (send_len == total_len)
			{
				//发送完毕
				delete[] send_buf;
				return kOk;
			}
		}
	}
	else
	{
		LOG_POWER_LOG_FATAL("tcp client have not connected.");
		return kNotConnctedToServer;
	}
}


void TcpClient::ResetClient()
{
	if (socket_fd_ != -1)
	{
		close(socket_fd_);
		socket_fd_ = -1;
	}
	if_connected_ = false;
}

TcpClient::~TcpClient()
{
	if (socket_fd_ != -1)
	{
		close(socket_fd_);
	}
}
