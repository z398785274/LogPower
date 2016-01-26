#ifndef PROCESS_LOG_READER_H
#define PROCESS_LOG_READER_H
/************************************************************************/
/* 处理log读取类
	对log文件进行读取，分割等
*/
/************************************************************************/

#include <string>
#include <stdio.h>
using namespace std;

#include "err_no.h"

class ProcessLogReader
{
public:
	ProcessLogReader(const string &path, int split_size);
	~ProcessLogReader();

public:
	/*
	从log文件中读取一段新的数据
	block:返回的内存地址
	len:内存长度
	返回值:见错误码解释
	*/
	ErrorValue ReadNexBlock(char **block, int &len);
	string GetLogFileName()
	{
		return file_name_;
	}
private:
	bool Open();
	bool Open(const string &file_path, bool for_move = false);
	void Close();
	void SplitLogFile();
	//移动log文件到一个新的名字，并返回
	bool MoveLogFile(string &new_file_path);
private:
	//待处理的日志的路径
	string process_log_path_;
	//切割日志大小
	int split_size_;
	FILE *file_;
	int current_file_len_;
	int last_pos_;
	string file_name_;
};
#endif

