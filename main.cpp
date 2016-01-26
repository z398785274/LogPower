#include <iostream>
#include <unistd.h>
#include <signal.h>
using namespace std;

#include "logger.h"
#include "config.h"

#include "process_log_reader.h"
#include "tcp_client.h"
#include "tcp_server.h"

logger *g_logger = NULL;
config *g_config = NULL;

const string k_process_log_file_path_key = "process_log_file_path";
const string k_process_log_split_size_key = "process_log_file_split_size";
const string k_server_ip_key = "server_ip";
const string k_server_port_key = "server_port";
int main(int argc, char *argv[])
{
	//服务类型，默认为client
	ServerType server_type = AsClient;
	if (argc >= 2 && strcasecmp(argv[1], "server") == 0)
	{
		LOG_POWER_LOG_INFO("as server type start...");
		server_type = AsServer;
		g_config = new config();
		if (!g_config || !g_config->Open("./config/log_power_server.config"))
		{
			LOG_POWER_LOG_FATAL("open log_power_server config file failed.");
			return -1;
		}
	}
	else
	{
		LOG_POWER_LOG_INFO("as client start.");
		g_config = new config();
		if (!g_config || !g_config->Open("./config/log_power_client.config"))
		{
			LOG_POWER_LOG_FATAL("open log_power_client config file failed.");
			return -1;
		}
	}

	string log_config_path;
	if (!g_config->GetStringValue("log_config_path", log_config_path))
	{
		LOG_POWER_LOG_FATAL("invalid config, have no log config path.");
		delete g_config;
		return -1;
	}
	g_logger = new logger(log_config_path.c_str());
	if (!g_logger || !g_logger->Init())
	{
		LOG_POWER_LOG_FATAL("g_logger init failed.");
		delete g_config;
		delete g_logger;
		return -1;
	}

	string server_ip;
	int server_port;
	if (!g_config->GetStringValue(k_server_ip_key, server_ip))
	{
		LOG_POWER_LOG_FATAL("get server ip failed.");
		return -1;
	}
	if (!g_config->GetIntValue(k_server_port_key, server_port))
	{
		LOG_POWER_LOG_FATAL("get server port failed.");
		return -1;
	}

	//当log文件达到split_size大小时，进行切割
	//单位是MB
	int process_log_file_split_size = 1024;
	if (!g_config->GetIntValue(k_process_log_split_size_key, process_log_file_split_size))
	{
		LOG_POWER_LOG_WARN("get process log file split size failed, use default 1024 MB");
	}

	signal(SIGPIPE, SIG_IGN);

	LOG_POWER_LOG_TRACE("will start to deal logical things...");

	if (server_type == AsClient)
	{
		//客户端
		//处理业务流程
		string process_log_file_path;
		if (!g_config->GetStringValue(k_process_log_file_path_key, process_log_file_path))
		{
			LOG_POWER_LOG_FATAL("get process log file path failed.");
			return -1;
		}

		ProcessLogReader log_reader(process_log_file_path, process_log_file_split_size);
		TcpClient tcp_client(server_ip, server_port);

		char *new_block = NULL;
		int new_block_len = 0;
		
		while (true)
		{
			//有可能是发送失败，需要重新连接
			if (tcp_client.ConnectToServer(2000) != kOk)
			{
				LOG_POWER_LOG_FATAL("connect to server failed, will reconnect after 2000ms");
				sleep(2);
				continue;
			}
			else
			{
				LOG_POWER_LOG_TRACE("connect to server successfully.");
			}
			
			if (new_block != NULL && new_block_len > 0)
			{
				//上次存在的内容，直接发送
				ErrorValue err_value = tcp_client.SendToServer(log_reader.GetLogFileName().c_str(), ( char*)new_block, new_block_len);
				if (err_value != kOk)
				{
					LOG_POWER_LOG_WARN("send block to server failed, will reset client and try again.");
					tcp_client.ResetClient();
					continue;
				}
				else
				{
					//发送完成
					LOG_POWER_LOG_TRACE("send info to server successfully.");
					delete[] new_block;
					new_block = NULL;
					new_block_len = 0;
				}

			}
			else
			{
				LOG_POWER_LOG_TRACE("read nothing...");
				sleep(5);
			}

			if (log_reader.ReadNexBlock(&new_block, new_block_len) == kOk)
			{
				//读取新的块到内存中,并释放cpu时间
				LOG_POWER_LOG_TRACE("read %d size from %s.", new_block_len, log_reader.GetLogFileName().c_str());
				sleep(2);
			}
			else
			{
				//读取下一块log文件失败，等待2s再读
				new_block = NULL;
				new_block_len = 0;
				LOG_POWER_LOG_FATAL("read next block failed, will wait for 5s ..");
				sleep(5);
			}
		}
		
		LOG_POWER_LOG_INFO("as client type end.");
	}
	else
	{
		//server处理
		TcpServer tcp_server(server_ip, server_port, process_log_file_split_size);
		if (tcp_server.LoadConfig() != kOk)
		{
			return 0;
		}
		if (tcp_server.StartServer() != kOk)
		{
			return 0;
		}

		//主线程在此等待进一步的命令
		while (true)
		{
			sleep(5);
		}
		LOG_POWER_LOG_INFO("as server type end.");
	}
	

	delete g_config;
	g_config = NULL;
	delete g_logger;
	g_logger = NULL;
	return 0;
}
