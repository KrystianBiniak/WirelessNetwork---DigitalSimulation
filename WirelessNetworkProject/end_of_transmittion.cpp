#include "end_of_transmittion.h"


EndOfTransmittion::EndOfTransmittion(size_t time, WirelessNetwork* wireless_network, EventList* event_list, int tx_id) : 
	Event(time, wireless_network, 1, nullptr), event_list_(event_list), wireless_network_(wireless_network), tx_id_(tx_id)
{
	tx_id_ = tx_id;
}

void EndOfTransmittion::Execute()
{
		wireless_network_->channel_->is_free = true;
		wireless_network_->is_transmission = false;
		wireless_network_->delayed_listenings_counter_ = 0;
		wireless_network_->EndTransmission(tx_id_);
		wireless_network_->transmitters.at(tx_id_)->ACK_sent = true;
		wireless_network_->transmitters.at(tx_id_)->BT = -1;
}


