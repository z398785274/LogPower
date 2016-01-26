#ifndef LOGPOWER_TCP_CLIENT_H
#define LOGPOWER_TCP_CLIENT_H
#include <string>
using namespace std;

#include "err_no.h"
/*
tcp 客户端类
非线程安全类
*/
class TcpClient
{
public:
	TcpClient(const string &ip, unsigned short port);
	~TcpClient();
public:
	/************************************************************************/
	/*对于已经建立了连接，并不会重复建立，仅仅只是检测状态，故每次发送前均可调用本函数.
	/************************************************************************/
	ErrorValue ConnectToServer(int time_out_in_ms);
	ErrorValue SendToServer(const char* log_name, const char* buf, int buf_len);
	//重置client的连接
	/************************************************************************/
	/* 当出现错误时，需要调用ResetClient将当前连接释放，并再次调用ConnectToServer进行连接。
	/************************************************************************/
	void ResetClient();
private:
	bool if_connected_;
	string ip_;
	unsigned short port_;

	//客户端fd
	int socket_fd_;
};
#endif
