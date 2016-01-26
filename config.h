#ifndef LOG_POWER_CONFIG_H
#define LOG_POWER_CONFIG_H
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <map>
using namespace std;

/*
配置文件处理类
每行支持的最大长度为1024
*/
class config
{
public:
	config()
	{}

	config(const char *file_path);
	~config();
	
	bool Open(const char *file_path);


	bool GetIntValue(const string &key, int &value)const
	{
		map<string, string>::const_iterator ite = config_content_.find(key);
		if (ite == config_content_.end())
		{
			return false;
		}
		else
		{
			value = atoi(ite->second.c_str());
			return true;
		}
	}


	bool GetStringValue(const string &key, string &value)const
	{
		map<string, string>::const_iterator ite = config_content_.find(key);
		if (ite == config_content_.end())
		{
			return false;
		}
		else
		{
			value = ite->second;
			return true;
		}
	}

private:
	map<string, string> config_content_;
};

#endif
