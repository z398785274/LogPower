#include "logger.h"
#include <stdio.h>
using namespace std;
using namespace log4cplus::helpers;


logger::logger(const char *property_file_path) :
	property_config_path_(property_file_path),
	init_flag_(false)
{
	
	
}

bool logger::Init()
{
	if (init_flag_)
	{
		return true;
	}

	log4cplus::initialize();
	//为了测试，先打开测试开关
	LogLog::getLogLog()->setInternalDebugging(true);
	PropertyConfigurator::doConfigure(property_config_path_);

	root_ = Logger::getRoot();
	private_logger_ = Logger::getInstance("logpower");

	init_flag_ = true;
}


void logger::LogTest(const char *msg, ...)
{
	if (!init_flag_)
	{
		return;
	}

 	va_list ap;
 	va_start(ap, msg);

	char buf[1024] = { 0 };
	vsnprintf(buf, 1024, msg, ap);
	LOG4CPLUS_INFO(private_logger_, buf);
	LOG4CPLUS_FATAL(root_, buf);
 	va_end(ap);
}

logger::~logger()
{

}
