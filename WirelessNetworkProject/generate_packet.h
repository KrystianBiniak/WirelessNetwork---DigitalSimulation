#ifndef GENERATE_PACKET_EVENT_H
#define GENERATE_PACKET_EVENT_H

#include "wireless_network.h"
#include "event.h"
#include "transmitter.h"
#include "logger.h"
#include "generator.h"

#include <functional>



class GeneratePacketEvent : public Event	//Event type is 3
{
public:
	GeneratePacketEvent(size_t time, WirelessNetwork* wireless_network, EventList* event_list, Transmitter* transmitter, Generator* generator);

	void Execute() override;

private:
	EventList* event_list_ = nullptr;
	int tx_id_ = 0;
	Transmitter* transmitter_ = nullptr;
	double time_ = 0;
	Logger logger_ = Logger();
	Generator* generator_ = nullptr;
};

#endif