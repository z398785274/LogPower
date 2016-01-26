#include "tool_functions.h"

#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include "md5.h"


int LOG_PWER::TimevalSubtract(timeval*   result, timeval*   x, timeval*   y)
{


	if (x->tv_sec > y->tv_sec)
		return   -1;

	if ((x->tv_sec == y->tv_sec) && (x->tv_usec > y->tv_usec))
		return   -1;

	result->tv_sec = (y->tv_sec - x->tv_sec);
	result->tv_usec = (y->tv_usec - x->tv_usec);

	if (result->tv_usec < 0)
	{
		result->tv_sec--;
		result->tv_usec += 1000000;
	}

	return   0;
}

void LOG_PWER::SleepMs(int inter)
{
	struct timeval timeout;
	timeout.tv_sec = inter / 1000;
	timeout.tv_usec = (inter % 1000) * 1000;
	select(0, 0, 0, 0, &timeout);
}

void LOG_PWER::Tokenize(const string &source, const string &separator, vector<string> &result_list)
{
	size_t left = 0;
	size_t pos = source.find(separator, left);
	size_t source_size = source.length();
	size_t separator_size = separator.size();
	string tmp;
	while (pos != string::npos)
	{
		//找到分割字符串的起始位置，将其左面的内容加入容器中
		tmp = source.substr(left, pos - left);
		if (!tmp.empty())
		{
			result_list.push_back(tmp);
		}

		//移动left指针
		left = pos + separator_size;

		//继续查找下一个
		pos = source.find(separator, left);
	}

	//把最后剩余的字段写入结果容器
	tmp = source.substr(left, source_size - left);
	if (!tmp.empty())
	{
		result_list.push_back(tmp);
	}

}


// BKDR Hash 
unsigned int LOG_PWER::BKDRHash(char *str)
{
	//seed按天进行计算
	time_t now;
	struct tm *timenow;
	time(&now);
	timenow = localtime(&now);
	unsigned int seed = timenow->tm_mday;
	unsigned int hash = 5;

	while (*str)
	{
		hash = hash * seed + (*str++);
	}

	return (hash & 0x7FFFFFFF);
}

void LOG_PWER::TrimAll(string &str)
{
	string::iterator ite;
	for (ite = str.begin(); ite != str.end();)
	{
		if ((*ite) == ' ')
		{
			ite = str.erase(ite);
		}
		else
			++ite;
	}
}

//生成md5值
void LOG_PWER::Md5Generator(const string &source, string &md5_str)
{
	MD5 md5(source);
	md5_str = md5.md5();
}

string LOG_PWER::GetProcName()
{
	pid_t pid = getpid();
	char exec_file[1024];
	char proc_name[1024];
	snprintf(exec_file, sizeof(exec_file), "/proc/%d/exe", pid);
	int ret = readlink(exec_file, proc_name, sizeof(proc_name));
	proc_name[ret] = 0;
	//获取文件名 
	char *p = strrchr(proc_name, '/');
	char name[256] = {0};
	snprintf(name, sizeof(name), "%s", p + 1);

	return name;
}

//获取路径中文件的名称
string LOG_PWER::GetFileName(const string &path)
{
	//获取文件名 
	char *p = strrchr(path.c_str(), '/');
	char name[256] = { 0 };
	snprintf(name, sizeof(name), "%s", p + 1);
	return name;
}


string LOG_PWER::GetCurrentTimeString()
{
	time_t now = time(NULL);
	tm *t = localtime(&now);
	char buf[100] = { 0 };
	strftime(buf, 100, "%Y%m%d%H%M%S", t);
	return buf;
}

int LOG_PWER::RunSystemCmd(const char *cmd)
{
	return system(cmd);
}

int LOG_PWER::SetNonBlock(int fd)
{	
	int flag = fcntl(fd, F_GETFL, NULL);
	flag = flag | O_NONBLOCK;
	return fcntl(fd, F_SETFL, flag);
}

int LOG_PWER::SetBlock(int fd)
{
	int flag = fcntl(fd, F_GETFL, NULL);
	flag = flag & ~O_NONBLOCK;
	return fcntl(fd, F_SETFL, flag);
}