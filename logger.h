#ifndef LOGGER_H
#define LOGGER_H
#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
using namespace std;
#include "log4cplus/helpers/loglog.h"
#include "log4cplus/helpers/stringhelper.h"
#include "log4cplus/loggingmacros.h"
#include "log4cplus/configurator.h"
using namespace log4cplus;

class logger;
extern logger *g_logger;


#define LOG_POWER_LOG_TRACE(fmt, arg...) \
{ \
if (g_logger && g_logger->IsInited()) \
{ \
	LOG4CPLUS_TRACE_FMT(g_logger->private_logger_, fmt, ## arg); \
} \
else \
{ \
	printf(fmt, ## arg); \
	printf("\n"); \
} \
}


#define LOG_POWER_LOG_DEBUG(fmt, arg...) \
{ \
	if (g_logger && g_logger->IsInited()) \
	{ \
		LOG4CPLUS_DEBUG_FMT(g_logger->private_logger_, fmt, ## arg); \
	} \
	else \
	{ \
	printf(fmt, ## arg); \
	printf("\n"); \
	} \
}

#define LOG_POWER_LOG_INFO(fmt, arg...) \
{ \
if (g_logger && g_logger->IsInited()) \
{ \
	LOG4CPLUS_INFO_FMT(g_logger->private_logger_, fmt, ## arg); \
} \
	else \
{ \
	printf(fmt, ## arg); \
	printf("\n"); \
} \
}

#define LOG_POWER_LOG_WARN(fmt, arg...) \
{ \
if (g_logger && g_logger->IsInited()) \
{ \
	LOG4CPLUS_WARN_FMT(g_logger->private_logger_, fmt, ## arg); \
} \
	else \
{ \
	printf(fmt, ## arg); \
	printf("\n"); \
} \
}


#define LOG_POWER_LOG_ERROR(fmt, arg...) \
{ \
if (g_logger && g_logger->IsInited()) \
{ \
	LOG4CPLUS_ERROR_FMT(g_logger->private_logger_, fmt, ## arg); \
} \
	else \
{ \
	printf(fmt, ## arg); \
	printf("\n"); \
} \
}


#define LOG_POWER_LOG_FATAL(fmt, arg...) \
{ \
if (g_logger && g_logger->IsInited()) \
{ \
	LOG4CPLUS_FATAL_FMT(g_logger->private_logger_, fmt, ## arg); \
} \
	else \
{ \
	printf(fmt, ## arg); \
	printf("\n"); \
} \
}


class logger
{
public:
	logger(const char *property_file_path);
	~logger();

	bool Init();

	bool IsInited()
	{
		return init_flag_;
	}
private:
	void LogTest(const char *msg, ...);
private:
	string property_config_path_;
	bool init_flag_;
public:
	Logger root_;
	Logger private_logger_;
};

#endif

