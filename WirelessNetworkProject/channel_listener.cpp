#include "channel_listener.h"

ChannelListener::ChannelListener(size_t clock, Event::EventList* event_list, WirelessNetwork* wireless_network, Transmitter* transmitter) 
	: Event(clock, wireless_network, 0, transmitter), clock_(clock), event_list_(event_list), wireless_network_(wireless_network), transmitter_(transmitter)
{
	event_list_ = event_list;
}

void ChannelListener::Execute()
{
	transmitter_->is_listening = false;
}
