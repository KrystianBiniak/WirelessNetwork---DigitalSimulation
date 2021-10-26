#include "logger.h"
#include <iostream>

Logger::Logger() {}
Logger::~Logger() {}



void Logger::Info(std::string message)
{
	if (event_level_ != EventLevel::Error)
	{
		std::cout << "[Info] " << message << std::endl;							//Output on cmd info about event that occured;
	}

}

void Logger::Debug(std::string message) {}

void Logger::Error(std::string message)
{
	std::cout << "[Error] " << message << std::endl;
}
