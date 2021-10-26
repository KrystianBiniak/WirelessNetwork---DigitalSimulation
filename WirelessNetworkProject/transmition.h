/*#ifndef TRANSMITION_H_
#define TRANSMITION_H_
#include "event.h"
#include "transmitter.h"
#include "logger.h"
#include "channel.h"
#include "receiver.h"



class Transmition : public Event, public Transmitter, public Channel
{
public:
	Transmition(int transmition_time, Logger* logger, Transmitter* transmitter, Channel* channel, Receiver* receiver);

	void Execute() override;

private:

	Receiver* receiver_ = nullptr;
	Transmitter* transmitter_ = nullptr;
	Logger* logger_ = nullptr;
	Channel* channel_ = nullptr;
	int DIFS = 1;
	int CWmax = 255;
	int CWmin = 15;
	int BT = 0;

};

#endif // !TRANSMITION_H_
*/