#include "config.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include "logger.h"
#include "tool_functions.h"

const int line_size = 1024;

config::config(const char* file_path)
{
	Open(file_path);
}


config::~config()
{
}

bool config::Open(const char *file_path)
{
	std::ifstream in_file(file_path, std::ios::in);

	char read_line[line_size] = { 0 };
	vector<string> lines;
	while (in_file.getline(read_line, line_size))
	{
		lines.push_back(read_line);
		memset(read_line, 0, 1024);
	}
	if (lines.size() == 0)
	{
		LOG_POWER_LOG_FATAL("open config file:%s failed.", file_path);
		return false;
	}

	config_content_.clear();

	for (size_t index = 0; index != lines.size(); index++)
	{
		string line = lines[index];
		//对于每一行进行处理
		if (line[0] == '#')
		{
			//注释
			continue;
		}

		//将空格去除掉
		LOG_PWER::TrimAll(line);
		if (line.length() == 0)
		{
			continue;
		}
	
		//非注释，则查找=号，则为分隔符
		size_t pos = line.find_first_of("=");
		if (pos == string::npos)
		{
			//不正确的配置
			LOG_POWER_LOG_INFO("invalid config line:%s", line.c_str());
			continue;
		}

		string key = line.substr(0, pos);
		string value = line.substr(pos + 1, line.length() - pos - 1);

		config_content_.insert(make_pair(key, value));

	}

	return true;
}

