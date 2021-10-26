#ifndef RECEIVER_H
#define RECEIVER_H
#include "logger.h"
#include "packet.h"
#include "transmitter.h"
#include <time.h>
#include <queue>



class Receiver  
{
public:
	Receiver(int identificator, Logger* logger);
	Receiver();

	std::queue<Packet*> packets_received;
	bool ACK;
private:
	int identificator_ = 0;
	int number_of_packets = 0;
	int transmition_time = 0;
	int CTIZ = 10;
	Logger* logger_ = nullptr;
	Transmitter* transmitter;
};

#endif	//	RECEIVER_H

