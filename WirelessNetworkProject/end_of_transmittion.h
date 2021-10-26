#ifndef END_OF_TRANSMITTION_H
#define END_OF_TRANSMITTION_H
#include "event.h"
#include "wireless_network.h"

class EndOfTransmittion	: public Event	//Event type is 1
{
public:
	EndOfTransmittion(size_t time, WirelessNetwork* wireless_network, EventList* event_list, int tx_id);

	void Execute() override;

private:
	EventList* event_list_ = nullptr;
	Transmitter* transmitter_ = nullptr;
	WirelessNetwork* wireless_network_ = nullptr;
	int tx_id_ = 0;
};

#endif	// END_OF_TRANSMITION_H