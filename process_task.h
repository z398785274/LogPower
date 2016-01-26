#ifndef LOGPOWER_PROCESSTASK_H
#define LOGPOWER_PROCESSTASK_H

#include <string>
#include <arpa/inet.h>
using namespace std;

#include "logger.h"
#include "err_no.h"

class TcpServer;
class ProcessTask
{
	friend class TcpServer;
public:
	ProcessTask(int split_size);
	~ProcessTask();

public:
	//内部属性处理
	inline void SetSocketFd(int socket_fd)
	{
		socket_fd_ = socket_fd;
	}
	inline int GetSocketFd()
	{
		return socket_fd_;
	}

	inline void SetClientIp(const char * addr)
	{
		client_ip_ = addr;
	}

	inline string GetClientIp()
	{
		return client_ip_;
	}

	inline void SetEpollFd(int epoll_fd)
	{
		epoll_fd_ = epoll_fd;
	}

	inline int GetEpollFd()
	{
		return epoll_fd_;
	}

public:
	ErrorValue ProcessReadTask();

private:
	void WriteToFile();
	void DeleteContentBuf()
	{
		if (content_buf_)
		{
			delete[] content_buf_;
			content_buf_ = NULL;
			content_len_ = 0;
			current_content_len_ = 0;

			//长度信息也需要重置
			len_buf_len_ = 0;
		}
	}

	//读取头信息
	/*
	kReadFailedOrPeerClosed:读取出错，退出
	kUnknown:未知错误，退出
	kBorderCheckFailed:检测边界失败，退出

	kReadEagain:继续读取

	kOk:正确处理了头部信息，读取到了要接收的内容长度，并为接收分配了内存
	*/
	ErrorValue ReadHeader();


	/*
	< 0:处理失败

	kReadEagain:继续读取

	kOk:正确读取了内容信息并写入了文件中，同时将缓冲区信息进行了重置
	*/
	ErrorValue ReadContent();

	/************************************************************************/
	/*
	return value:
	kReadFailedOrPeerClosed:读取发生错误或者peer closed，删除task
	kReadEagain:eagin，继续读
	kOk:读取完毕
	*/
	/************************************************************************/
	ErrorValue ReadFromSocket(const char *read_buf, int read_buf_size, int &real_read_size);
private:
	int socket_fd_;
	string client_ip_;
	int epoll_fd_;

	FILE *file;

	//接收到的内容
	char *content_buf_;
	int content_len_;
	int current_content_len_;

	//长度处理buf
	char *len_buf_;
	int len_buf_len_;

	int split_size_;

	bool if_in_epoll_;
};
#endif

