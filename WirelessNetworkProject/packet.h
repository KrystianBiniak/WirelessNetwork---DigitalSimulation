#ifndef PACKET_H_
#define PACKET_H_
#include <iostream>
#include <string>

class Packet
{
public:
	Packet(size_t id, int tx_id);

	bool lost;

	size_t get_id() { return id_; }
	int GetPacketsTxId();
	bool delivered = false;
	int packet_retransmissions = 0;
	size_t start_clock_ = 0;
	size_t generation_time = 0;
	size_t GetGenerationTime() { return generation_time; }

private:
	size_t id_ = 0;
	int tx_id_ = 0;
};

#endif	// PACKET_H_