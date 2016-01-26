#include "process_log_reader.h"
#include <errno.h>
#include <string.h>

#include "tool_functions.h"
#include "logger.h"
using namespace LOG_PWER;

ProcessLogReader::ProcessLogReader(const string &path, int split_size)
: process_log_path_(path)
, file_(NULL)
, current_file_len_(0)
, last_pos_(0)
{
	file_name_ = GetFileName(path);
	split_size_ = split_size * 1024 * 1024;
}


ProcessLogReader::~ProcessLogReader()
{
}

bool ProcessLogReader::Open()
{
	file_ = fopen(process_log_path_.c_str(), "r");
	if (file_ == NULL)
	{
		LOG_POWER_LOG_FATAL("open %s failed:%s", process_log_path_.c_str(), strerror(errno));
		return false;
	}
	fseek(file_, 0, SEEK_END);
	current_file_len_ = ftell(file_);

	return true;
}


bool ProcessLogReader::Open(const string &file_path, bool for_move)
{
	if (!for_move)
	{
		file_name_ = GetFileName(file_path);
	}
	file_ = fopen(file_path.c_str(), "r");
	if (file_ == NULL)
	{
		LOG_POWER_LOG_FATAL("open %s failed:%s", file_path.c_str(), strerror(errno));
		return false;
	}
	fseek(file_, 0, SEEK_END);
	current_file_len_ = ftell(file_);

	return true;
}

void ProcessLogReader::Close()
{
	fclose(file_);
	file_ = NULL;
	current_file_len_ = 0;
	return;
}


bool ProcessLogReader::MoveLogFile(string &new_file_path)
{
	new_file_path = process_log_path_ + "_" + GetCurrentTimeString();
	char cmd_buf[256] = {0};
	snprintf(cmd_buf, 256, "mv %s %s", process_log_path_.c_str(), new_file_path.c_str());
	//执行移动命令
	if (RunSystemCmd((const char*)cmd_buf) == -1)
	{
		LOG_POWER_LOG_FATAL("mv %s to %s failed.", process_log_path_.c_str(), new_file_path.c_str());
		return false;
	}

	LOG_POWER_LOG_INFO("mv %s to %s successfully.", process_log_path_.c_str(), new_file_path.c_str());
	return true;
}

ErrorValue ProcessLogReader::ReadNexBlock(char **block, int &len)
{
	if (NULL == block)
	{
		return kInvalidParams;
	}

	len = 0;
	*block = NULL;
	
	if (!Open())
	{
		//打开失败
		return kOpenFileFailed;
	}

	//log文件处于打开状态
	if (current_file_len_ < last_pos_)
	{
		//新的文件长度比原来读取的位置还小，则说明文件被新改动过,则需要从头读取
		LOG_POWER_LOG_TRACE("read new file...");
		last_pos_ = 0;
	}

	if (current_file_len_ == last_pos_)
	{
		//没有读取到新的内容
		LOG_POWER_LOG_TRACE("log file is not changed...");
		Close();
		return kOk;
	}
	//if(current_file_len_ > last_pos_),如果文件被重新大量写入造成的长度暴增，无法解决。

	if (current_file_len_ >= split_size_)
	{
		//当前log文件长度已经大于了切割长度，则需要进行切割
		LOG_POWER_LOG_TRACE("need to split file...");
		Close();
		//转移当前log文件
		string new_file_path;
		if (!MoveLogFile(new_file_path))
		{
			return kRunSystemCmdFailed;
		}

		//打开新的文件
		if (!Open(new_file_path, true))
		{
			LOG_POWER_LOG_TRACE("open moved file failed...");
			return kOpenFileFailed;
		}
	}

	len = current_file_len_ - last_pos_;
	//如果大于100MB，则按100MB进行划分
	if (len > g_100MB)
	{
		len = g_100MB;
	}
	*block = new char[len];
	fseek(file_, last_pos_, SEEK_SET);
	fread(*block, len, 1, file_);
	last_pos_ += len;
	
	Close();

	return kOk;
}
