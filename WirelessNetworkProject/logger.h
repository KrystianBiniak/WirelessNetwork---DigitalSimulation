#ifndef LOGGER_H_
#define LOGGER_H_
#include <string>
#include <iostream>
//#include <fstream>

class Logger
{
public:
	enum class EventLevel { Info, Debug, Error};
	Logger();
	~Logger();

	void set_event_level(EventLevel event_level)
	{
		event_level_ = event_level;
	}

	void Info(std::string message);
	void Debug(std::string message);
	void Error(std::string message);
private:
	EventLevel event_level_ = EventLevel::Info;
};

#endif

