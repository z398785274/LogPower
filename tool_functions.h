#ifndef TOOL_FUNCTION_H
#define TOOL_FUNCTION_H
#include <string>
#include <vector>
#include <sys/time.h>
using namespace std;


const int g_100MB = 1024 * 1024 * 100;

namespace LOG_PWER
{

	//分词器函数
	/*
	source:词源
	separator:分隔符
	result_list:分词结果存储容器

	此函数中，仅作精确匹配，并不会做额外处理，如去空白符等。
	result_list不会首先清空再使用
	*/
	void Tokenize(const string &source, const string &separator, vector<string> &result_list);


	void SleepMs(int inter);



	/**
	*   计算两个时间的间隔，得到时间差
	*   @param   struct   timeval*   resule   返回计算出来的时间
	*   @param   struct   timeval*   x             需要计算的前一个时间
	*   @param   struct   timeval*   y             需要计算的后一个时间
	*   return   -1   failure   ,0   success
	**/
	int   TimevalSubtract(timeval*   result, timeval*   x, timeval*   y);

	//将字符串进行hash
	unsigned int BKDRHash(char *str);

	//去除string中的所有空白
	void TrimAll(string &str);


	//生成md5值
	void Md5Generator(const string &source, string &md5_str);

	//获取应用的名称
	string GetProcName();

	//获取路径中文件的名称
	string GetFileName(const string &path);


	//获取当前的时间，格式为"%Y%m%d%H%M%S"
	string GetCurrentTimeString();


	//执行系统命令
	int RunSystemCmd(const char *cmd);

	//将fd设置为no block
	int SetNonBlock(int fd);

	//将fd设置为block
	int SetBlock(int fd);
};
#endif

