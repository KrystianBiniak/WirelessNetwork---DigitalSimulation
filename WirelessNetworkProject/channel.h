#ifndef CHANNEL_H
#define CHANNEL_H
#include "logger.h"
#include "packet.h"
#include <queue>

class Channel
{
public:
	Channel(Logger* logger);
	Channel();

	bool is_free;
	std::queue<Packet*> channel_buffer;

private:
	Logger* logger_ = nullptr;
	int ter_value_;

	
};

#endif	//	CHANNEL_H

