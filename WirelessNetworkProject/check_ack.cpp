#include "check_ack.h"
#include <algorithm>
#include <iostream>

CheckACK::CheckACK(size_t time, WirelessNetwork* wireless_network, EventList* event_list, Transmitter* transmitter) : 
	Event(time, wireless_network, 2, transmitter), event_list_(event_list), wireless_network_(wireless_network), transmitter_(transmitter)
{
	wireless_network_->ACK_check_flag = true;
	transmitter->busy = false;
	transmitter->BT = 0;
}


void CheckACK::Execute()
{
	transmitter_->ACK_sent = false;
	logger.set_event_level(Logger::EventLevel::Info);
}