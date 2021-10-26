#include "event.h"

Event::Event(size_t event_time, WirelessNetwork* wireless_network, int type, Transmitter* transmitter) 
	: event_time_(event_time), wireless_network_(wireless_network), event_type_(type), transmitter_(transmitter)
{
}

int Event::GetEventType()
{
	return event_type_;
}

