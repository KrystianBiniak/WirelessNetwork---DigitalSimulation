#ifndef EVENT_H_
#define EVENT_H_
#include <vector>
#include <functional>
#include <queue>
#include "transmitter.h"

class WirelessNetwork;

class Event
{
public:
	typedef std::priority_queue<Event*, std::vector<Event*>, std::function<bool(Event*, Event*)>> EventList;
	Event(size_t event_time, WirelessNetwork* wireless_network, int type, Transmitter* transmitter);


	virtual void Execute() = 0;

	size_t get_time() { return event_time_; }
	int GetEventType();
	Transmitter* transmitter_ = nullptr;

protected:
	size_t event_time_ ;
	int event_type_;
	WirelessNetwork* wireless_network_ = nullptr;
};

#endif