#include "packet.h"
#include <iostream>
#include <string>

Packet::Packet(size_t id, int tx_id) : id_(id), tx_id_(tx_id)
{
}

int Packet::GetPacketsTxId()
{
	return tx_id_;
}