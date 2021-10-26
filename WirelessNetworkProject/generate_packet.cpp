#include "generate_packet.h"
#include "wireless_network.h"
#include <string>


GeneratePacketEvent::GeneratePacketEvent(size_t time, WirelessNetwork* wireless_network, EventList* event_list, Transmitter* transmitter, Generator* generator) 
	: Event(time, wireless_network, 3, transmitter), event_list_(event_list), transmitter_(transmitter), time_(time), generator_(generator)
{
	std::string tx_id_s = std::to_string(transmitter_->GetTxId()+1);
	time_ /= 10;
}


void GeneratePacketEvent::Execute()
{
	wireless_network_->GeneratePacket(transmitter_, event_list_, time_, generator_ );
}

