#ifndef CHECK_ACK_H
#define CHECK_ACK_H
#include "wireless_network.h"
#include "end_of_transmittion.h"
#include "logger.h"
#include "event.h"

class CheckACK : public Event	//Event type is 2
{
public:
	CheckACK(size_t time, WirelessNetwork* wireless_network, EventList* event_list, Transmitter* transmitter);
	
	void Execute() override;

	Logger logger = logger;



private:
	EventList* event_list_ = nullptr; 
	WirelessNetwork* wireless_network_ = nullptr;
	Transmitter* transmitter_ = nullptr;
};

#endif // !CHECK_ACK_H
