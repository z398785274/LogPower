#include "process_task.h"

#include <errno.h>


#include "power.loginfo.pb.h"
#include "logger.h"
#include "tool_functions.h"

using namespace LOG_PWER;

ProcessTask::ProcessTask(int split_size):
file(NULL),
content_buf_(NULL),
content_len_(0),
len_buf_(NULL),
if_in_epoll_(false)
{
	len_buf_ = new char[8];
	len_buf_len_ = 0;
	split_size_ = split_size * 1024 * 1024;
	socket_fd_ = -1;
}


ProcessTask::~ProcessTask()
{
	if (len_buf_)
	{
		delete[] len_buf_;
		len_buf_ = NULL;
	}

	if (content_buf_)
	{
		delete[] content_buf_;
		content_buf_ = NULL;
	}
	if (file)
	{
		fclose(file);
		file = NULL;
	}

	if (socket_fd_ != -1)
	{
		close(socket_fd_);
	}
	
}

// int ProcessTask::DealReceivedBuf(const char* buf, int buf_len)
// {
// 	if (len_buf_len_ < 8)
// 	{
// 		LOG_POWER_LOG_TRACE("now len_buf_len_ is:%d, will deal length data, buf len:%d", len_buf_len_, buf_len);
// 		//处理长度相关
// 		if ((len_buf_len_ + buf_len) < 8)
// 		{
// 			//全部是长度相关，无实际内容
// 			memcpy(len_buf_ + len_buf_len_, buf, buf_len);
// 			len_buf_len_ += buf_len;
// 			LOG_POWER_LOG_TRACE("received len content is less than 8,will continue to receive.");
// 			return 1;
// 		}
// 		else
// 		{
// 			//包含了长度和数据
// 			memcpy(len_buf_ + len_buf_len_, buf, 8 - len_buf_len_);
// 			data_pointer_ = buf + (8 - len_buf_len_);
// 			data_len_ = buf_len - (8 - len_buf_len_);
// 			len_buf_len_ = 8;
// 			LOG_POWER_LOG_TRACE("received enough len content.");
// 		}
// 	}
// 
// 	//为实际数据申请内存
// 	if (content_buf_ == NULL)
// 	{
// 		LOG_POWER_LOG_TRACE("content is null.");
// 		//需要判断buf，是否符合正确的格式.
// 		//前四个字节是border_num
// 		int32_t border_num;
// 		memcpy(&border_num, len_buf_, 4);
// 		border_num = ntohl(border_num);
// 		if (border_num != 323297739)
// 		{
// 			len_buf_len_ = 0;
// 			LOG_POWER_LOG_FATAL("border num is wrong:%d,abandon.", border_num);
// 			return -1;
// 		}
// 
// 		//获取实际的消息长度并分配内存
// 		int32_t body_len;
// 		memcpy(&body_len, len_buf_ + 4, 4);
// 		body_len = ntohl(body_len);
// 		content_buf_ = new char[body_len];
// 		content_len_ = body_len;
// 		
// 		LOG_POWER_LOG_TRACE("received body len is:%d", body_len);
// 
// 		return 2;
// 	}
// 	else
// 	{
// 		LOG_POWER_LOG_TRACE("content is not null.");
// 		//当前content_buf中保存的数据长度为data_len_.
// 		if (data_len_ + buf_len > content_len_)
// 		{
// 			//接收到了多余的数据
// 			return 3;
// 		}
// 		else if (data_len_ + buf_len == content_len_)
// 		{
// 			//正好
// 			return 4;
// 		}
// 		else
// 		{
// 			//数据尚未接收足够
// 			return 5;
// 		}
// 
// 	}
// }

// void ProcessTask::ProcessReadBuf(const char *buf, int buf_len)
// {
// 	while (true)
// 	{
// 		int result = DealReceivedBuf(buf, buf_len);
// 		if (result == -1)
// 		{
// 			return;
// 		}
// 		if (result == 1)
// 		{
// 			//需要继续接受长度数据
// 			break;
// 		}
// 		if (result == 2)
// 		{
// 			//分配了内存，可以继续读取数据部分
// 			if (data_len_ > content_len_)
// 			{
// 				LOG_POWER_LOG_TRACE("received data is more than content len.");
// 				//接收到了一部分下个包的内容
// 				memcpy(content_buf_, data_pointer_, content_len_);
// 
// 				buf = data_pointer_ + content_len_;
// 				buf_len = data_len_ - content_len_;
// 				//处理接收到的信息
// 				WriteToFile();
// 				DeleteContentBuf();
// 
// 				//继续处理下一部分
// 				continue;
// 			}
// 			else if (data_len_ == content_len_)
// 			{
// 				LOG_POWER_LOG_TRACE("received data is equal to content len.");
// 				memcpy(content_buf_, data_pointer_, content_len_);
// 				//本包全部接收到了,并未多余
// 				WriteToFile();
// 				DeleteContentBuf();
// 				break;
// 			}
// 			else
// 			{
// 				LOG_POWER_LOG_TRACE("received data is less than content len, need to continue to receive.");
// 				//本包未收全,继续收取，等待下次进行处理
// 				memcpy(content_buf_, data_pointer_, data_len_);
// 				break;
// 			}
// 		}
// 		if (result == 3)
// 		{
// 			LOG_POWER_LOG_TRACE("and pre content, more than content len.");
// 			//接收到了多余的数据
// 			memcpy(content_buf_ + data_len_, buf, content_len_ - data_len_);
// 			buf = buf + (content_len_ - data_len_);
// 			buf_len = buf_len - (content_len_ - data_len_);
// 			WriteToFile();
// 			DeleteContentBuf();
// 			continue;
// 		}
// 		if (result == 4)
// 		{
// 			LOG_POWER_LOG_TRACE("and pre content, is equal to content len.");
// 			memcpy(content_buf_ + data_len_, buf, content_len_ - data_len_);
// 			WriteToFile();
// 			DeleteContentBuf();
// 			break;
// 		}
// 		if (result == 5)
// 		{
// 			LOG_POWER_LOG_TRACE("and pre content, is less than content len.");
// 			memcpy(content_buf_ + data_len_, buf, buf_len);
// 			data_len_ += buf_len;
// 			break;
// 		}
// 	}
// 	return;
// }

void ProcessTask::WriteToFile()
{
	power::loginfo info;
	if (!info.ParseFromArray(content_buf_, content_len_))
	{
		LOG_POWER_LOG_FATAL("parse from array failed.");
		return;
	}
	string file_name = client_ip_ + "_" + info.log_name();
	if (file == NULL)
	{
		file = fopen(file_name.c_str(), "a");
	}

	if (file == NULL)
	{
		LOG_POWER_LOG_FATAL("process read buf failed:open file:%s failed:%s.", file_name.c_str(), strerror(errno));
		return;
	}
	LOG_POWER_LOG_TRACE("will write to file:%s", file_name.c_str());
	fwrite(info.log_content().c_str(), info.log_len(), 1, file);
	fflush(file);
	int cuurent_file_len = ftell(file);
	if (cuurent_file_len >= split_size_)
	{
		fclose(file);
		file = NULL;
		string new_file_path = file_name + "_" + GetCurrentTimeString();
		char cmd_buf[256] = { 0 };
		snprintf(cmd_buf, 256, "mv %s %s", file_name.c_str(), new_file_path.c_str());
		//执行移动命令
		if (RunSystemCmd((const char*)cmd_buf) == -1)
		{
			LOG_POWER_LOG_FATAL("mv %s to %s failed.", file_name.c_str(), new_file_path.c_str());
			return ;
		}
		else
		{
			LOG_POWER_LOG_FATAL("mv %s to %s successfully.", file_name.c_str(), new_file_path.c_str());
			return;
		}
	}
	return;
}


ErrorValue ProcessTask::ReadFromSocket(const char *read_buf, int read_buf_size, int &real_read_size)
{
	real_read_size = 0;
	while (true)
	{
		int read_size = recv(socket_fd_, (void*)(read_buf + real_read_size), read_buf_size - real_read_size, 0);
		LOG_POWER_LOG_TRACE("epoll　fd:%d read %d size from client fd:%d", epoll_fd_, read_size, socket_fd_);
		if (read_size <= 0)
		{
			//读出错
			if (read_size < 0)
			{
				int err_no;
				socklen_t len = sizeof(err_no);
				if (getsockopt(socket_fd_, SOL_SOCKET, SO_ERROR, &err_no, &len) < 0)
				{
					LOG_POWER_LOG_FATAL("get socket opt failed:%s", strerror(errno));
					err_no = errno;
				}
				if (err_no == EINTR)
				{
					//终端信号，继续读
					LOG_POWER_LOG_INFO("interrupt,continue.");
					LOG_POWER_LOG_TRACE("epoll　fd:%d read interuppt signal from client fd:%d", epoll_fd_, socket_fd_);
					continue;
				}
				else if (err_no == EAGAIN || err_no == 0)
				{
					//当前块中的数据读取完毕，等待下次继续读
					LOG_POWER_LOG_TRACE("%s have eagain info", client_ip_.c_str());
					LOG_POWER_LOG_TRACE("epoll　fd:%d read eagain from client fd:%d", epoll_fd_, socket_fd_);
					return kReadEagain;
	
				}
				else
				{
					//读取失败
					LOG_POWER_LOG_FATAL("read data from client:%s failed:%s", client_ip_.c_str(), strerror(errno));
					LOG_POWER_LOG_TRACE("epoll　fd:%d read failed from client fd:%d", epoll_fd_, socket_fd_);
					return kReadFailedOrPeerClosed;
				}
			}
			else
			{
				//peer closed
				LOG_POWER_LOG_WARN("peer:%s closed", client_ip_.c_str());
				LOG_POWER_LOG_TRACE("epoll　fd:%d read peer closed from client fd:%d", epoll_fd_, socket_fd_);
				return kReadFailedOrPeerClosed;
			}
		}
		else
		{
			//读取到实际数据
			real_read_size += read_size;
			if (read_buf_size == real_read_size)
			{
				//本次需要读取的数据，全部读取完毕
				return kOk;
			}
			else
			{
				//继续读取
				LOG_POWER_LOG_TRACE("continue to read, need to read size is:%d, real read size is %d", read_buf_size, real_read_size);
				continue;
			}
		}
	}
}


ErrorValue ProcessTask::ReadHeader()
{
	int real_read_size = 0;
	ErrorValue result = ReadFromSocket(len_buf_ + len_buf_len_, 8 - len_buf_len_, real_read_size);
	len_buf_len_ += real_read_size;

	if (result != kOk)
	{
		return result;
	}

	//header读取完毕
	if (len_buf_len_ != 8)
	{
		LOG_POWER_LOG_FATAL("read len buf len is not 8, will abandon this task");
		return kUnknown;
	}

	//解析长度信息
	//需要判断buf，是否符合正确的格式.
	//前四个字节是border_num
	int32_t border_num;
	memcpy(&border_num, len_buf_, 4);
	border_num = ntohl(border_num);
	if (border_num != 323297739)
	{
		len_buf_len_ = 0;
		LOG_POWER_LOG_FATAL("border num is wrong:%d,abandon.", border_num);
		return kBorderCheckFailed;
	}

	//获取实际的消息长度并分配内存
	int32_t body_len;
	memcpy(&body_len, len_buf_ + 4, 4);
	body_len = ntohl(body_len);

	//content buf确保使用完毕后，已经删除
	content_buf_ = new char[body_len];
	content_len_ = body_len;
	current_content_len_ = 0;

	LOG_POWER_LOG_TRACE("received body len is:%d", body_len);

	return kOk;
	
}

ErrorValue ProcessTask::ReadContent()
{
	int real_read_size = 0;
	ErrorValue return_value = ReadFromSocket(content_buf_ + current_content_len_, content_len_ - current_content_len_, real_read_size);
	current_content_len_ += real_read_size;

	if (return_value != kOk)
	{
		return return_value;
	}
	
	if (current_content_len_ != content_len_)
	{
		LOG_POWER_LOG_FATAL("current content len is not eqaul to content len.");
		return kUnknown;
	}

	//读取完毕，对读取到的内容进行处理
	WriteToFile();
	DeleteContentBuf();
	return kOk;
}

ErrorValue ProcessTask::ProcessReadTask()
{
	ErrorValue return_value;

	//循环处理一个一个的数据包
	while (true)
	{
		if (len_buf_len_ < 8)
		{
			//读取头部信息
			return_value = ReadHeader();
			if (return_value != kOk)
			{
				return return_value;
			}
		}
		//对read task进行处理
		return_value = ReadContent();
		if (return_value != kOk)
		{
			return return_value;
		}
	}

}