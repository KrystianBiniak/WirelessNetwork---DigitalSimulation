#ifndef CHANNEL_LISTENER_H
#define CHANNEL_LISTENER_H
#include "channel.h"
#include "event.h"
#include "transmitter.h"
#include "wireless_network.h"
#include "logger.h"

class ChannelListener	:	public Event	//Event type is 0
{
public:
	ChannelListener(size_t clock_, Event::EventList* event_list, WirelessNetwork* wireless_network, Transmitter* transmitter);

	void Execute() override;

private:
	Logger logger_ = Logger();
	Transmitter* transmitter_ = nullptr;
	EventList* event_list_ = nullptr;
	WirelessNetwork* wireless_network_ = nullptr;
	size_t clock_ = 0;
};

#endif // !CHANNEL_LISTENER_H
