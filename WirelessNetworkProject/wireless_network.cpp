#include "wireless_network.h"
#include "channel_listener.h"
#include "check_ack.h"
#include "end_of_transmittion.h"
#include "generator.h"
#include <iostream>
#include <string>
#include <fstream>
#include <iterator>
#include <tgmath.h>



WirelessNetwork::WirelessNetwork() {}

WirelessNetwork::~WirelessNetwork() {}

void WirelessNetwork::CreateNetwork(Event::EventList* event_list)
{
	logger.set_event_level(Logger::EventLevel::Info);
	srand(time(NULL));
	logger.Info("Choose kernels set(1-10): ");
	std::cin >> kernels_set_;
	if (kernels_set_ <= 1 || kernels_set_ > 10) read_kernel.open("kernels1.txt");	//Randomly select first set
	if (kernels_set_ == 2) read_kernel.open("kernels2.txt");
	if (kernels_set_ == 3) read_kernel.open("kernels3.txt");
	if (kernels_set_ == 4) read_kernel.open("kernels4.txt");
	if (kernels_set_ == 5) read_kernel.open("kernels5.txt");
	if (kernels_set_ == 6) read_kernel.open("kernels6.txt");
	if (kernels_set_ == 7) read_kernel.open("kernels7.txt");
	if (kernels_set_ == 8) read_kernel.open("kernels8.txt");
	if (kernels_set_ == 9) read_kernel.open("kernels9.txt");
	if (kernels_set_ == 10) read_kernel.open("kernels10.txt");

	while (read_kernel >> kernel_s)
	{
		kernel = std::stoi(kernel_s);
		kernels.push(kernel);
	}

	for (current_tx_id = 0; current_tx_id < K; ++current_tx_id)
	{
		Generator* generator = nullptr;
		for (int i = 0; i < 3; ++i)
		{
			generator = new Generator(kernels.front(), current_tx_id, i);	//Rand, Rand(BT), expL generators
			kernels.pop();
			if (i == 0)
			{
				CGPk_ = 10 * floor(generator->RandExp(L));
				CGPk_gen = generator;	//All CGPk generators have id == 0 and its tx's id
			}
			if (i == 1) CTPk_gen = generator;	//All CTPk generatos have id == 1 and its tx's id
			if (i == 2) BT_gen = generator;		//All BT generators have id == 2 and its tx's id
		}
		transmitters.push_back(new Transmitter(current_tx_id, &logger, CGPk_gen, CTPk_gen, BT_gen));				//Creating K-number of transmitter
		Transmitter* transmitter = transmitters.at(current_tx_id);
		event_list->push(new GeneratePacketEvent(CGPk_, this, event_list, transmitter, CGPk_gen));
		receivers.push_back(new Receiver(current_tx_id, &logger));								//Creating receivers
	}

	TER_gen = new Generator(kernels.front(), 999, 999);		//TER Generator with unique ids							
	kernels.pop();

	Channel* channel = new Channel(&logger);												//Creating Channel
	channel_ = channel;

	channel_->is_free = true;
	generate_timer = -1;
	transmission_timer = -1;

}

void WirelessNetwork::GeneratePacket(Transmitter* transmitter, Event::EventList* event_list, size_t time, Generator* generator)
{
	++id;
	std::string id_s = std::to_string(id);
	auto packet = new Packet(id, transmitter->GetTxId());
	packet->generation_time = time;
	transmitter->buffer.push(packet);
	std::string tx_id_s = std::to_string(transmitter->GetTxId()+1);
	logger.set_event_level(Logger::EventLevel::Info);
	logger.Info("Generated packet id: " +id_s+ ", its tx's id: "+tx_id_s);
	CGPk_ = time * 10;;
	CGPk_ += 10 * generator->RandExp(L);
	CGPk_ /= 10;
	CGPk_ = 10 * round(CGPk_);
	event_list->push(new GeneratePacketEvent(CGPk_, this, event_list, transmitter, generator));
}

void WirelessNetwork::BeginTransmission(Event::EventList* event_list, int tx_id, size_t clock)
{
		logger.set_event_level(Logger::EventLevel::Info);
		std::string transmission_number_s = std::to_string(transmission_number_);
		std::string retransmission_counter_s = std::to_string(retransmissions_counter);
		std::string tx_id_s = std::to_string(tx_id + 1);
		if (transmitters.at(tx_id)->is_retransmission_ == false && transmitters.at(tx_id)->retransmission_packet == nullptr && channel_->is_free)
		{
			avg_wait_time += (clock - transmitters.at(tx_id)->buffer.front()->generation_time) / 10;
			sent_packets_counter += 1;
			initial_phase_ += 1;
			std::string packet_id_s = std::to_string(transmitters.at(tx_id)->buffer.front()->get_id());
			transmitters.at(tx_id)->buffer.front()->start_clock_ = clock;
			channel_->channel_buffer.push(transmitters.at(tx_id)->buffer.front());
			transmitters.at(tx_id)->buffer.pop();
			++transmission_number_;
			logger.Info("Transmission number " + transmission_number_s + " began, tx id is: " + tx_id_s + ",packet id: " +packet_id_s);	//Wyœwietl, która transmisja siê rozpoczê³a
			transmitters.at(tx_id)->BT = -1;
			SetACK();	//Uwzglêdnij prawdopodobieñstwo TER przy fladze ACK
			rand_time_transmission_ = clock + 10 * transmitters.at(tx_id)->CTPk_gen_->RandSection(1, 10);
			time_ACK_ = rand_time_transmission_ + 10;
			event_list->push(new EndOfTransmittion(rand_time_transmission_, this, event_list, tx_id));
			event_list->push(new CheckACK(time_ACK_, this, event_list, transmitters.at(tx_id)));
			is_transmission = true;
		}
		//INITIAL PHASE INCLUDED
		if (initial_phase_ == 250 && sent_packets_counter == 250)
		{
			sent_packets_counter = 0;
			received_packets_counter = 0;
			lost_packets_counter = 0;
			is_packet_waiting_for_retransmission_counter = 0;
			mean_packets_transmission_time = 0;
			retransmissions_counter = 0;
			total_retransmissions_counter = 0;
			colissions_counter = 0;
			TER_counter_ = 0;
			first_collission_counter = 0;
			retransmision_and_colission_counter = 0;
		}
		if (transmitters.at(tx_id)->is_retransmission_ == true && channel_->is_free && transmitters.at(tx_id)->retransmission_packet != nullptr)
		{
			transmitters.at(tx_id)->retransmission_packet->packet_retransmissions++;
			if (transmitters.at(tx_id)->retransmission_packet->packet_retransmissions < 2)
			{
				++retransmissions_counter;
			}
			total_retransmissions_counter++;
			std::string packet_id_s = std::to_string(transmitters.at(tx_id)->retransmission_packet->get_id());
			channel_->channel_buffer.push(transmitters.at(tx_id)->retransmission_packet);
			transmitters.at(tx_id)->retransmission_packet = nullptr;
			logger.Info("Retransimission number " + retransmission_counter_s + " began, tx id is: " + tx_id_s + ",packet id: " + packet_id_s);
			transmitters.at(tx_id)->BT = -1;
			SetACK();	//Uwzglêdnij prawdopodobieñstwo TER przy fladze ACK
			rand_time_transmission_ = clock + 10 * transmitters.at(tx_id)->CTPk_gen_->RandSection(1, 10);
			time_ACK_ = rand_time_transmission_ + 10;
			event_list->push(new EndOfTransmittion(rand_time_transmission_, this, event_list, tx_id));
			event_list->push(new CheckACK(time_ACK_, this, event_list, transmitters.at(tx_id)));
			is_transmission = true;
		}
		channel_->is_free = false;
}

void WirelessNetwork::EndTransmission(int tx_id)
{
	std::string transmission_number_s = std::to_string(transmission_number_ - 1);
	std::string retransmission_counter_s = std::to_string(retransmissions_counter - 1);
	logger.set_event_level(Logger::EventLevel::Info);
	if (transmitters.at(tx_id)->is_retransmission_ == false)  logger.Info("Transmission number " + transmission_number_s + " ended");		//Wyœwietl, która transmisja siê skoñczy³a
	if (transmitters.at(tx_id)->is_retransmission_ == true)  logger.Info("Retransmission number " + retransmission_counter_s + " ended");
}

void WirelessNetwork::SetACK() 
{
	TER_return_ = TER_gen->RandZeroOne(P_);
	if (TER_return_ == 1) ack = true;
	else ack = false;
}

void WirelessNetwork::StartListening(Event::EventList* event_list, size_t clock_, int tx_id)
{
		if( !transmitters.at(tx_id)->buffer.empty())
		event_list->push(new ChannelListener(clock_ + 5, event_list, this, transmitters.at(tx_id)));
}

void WirelessNetwork::CheckIfIsColission(int tx_id)
{
	logger.set_event_level(Logger::EventLevel::Error);
	std::string tx_id_s = std::to_string(tx_id + 1);
	for (int i = 0; i < K; i++)
	{
		if (i != tx_id)
		{
			if ( transmitters.at(i)->BT == transmitters.at(tx_id)->BT && transmitters.at(tx_id)->BT >= 0 && transmitters.at(i)->BT >= 0 && !transmitters.at(i)->buffer.empty() )
			{
				transmitters.at(tx_id)->collision = true;
				std::string packet_id_s;
				if (!transmitters.at(tx_id)->is_retransmission_)
				{
					packet_id_s = std::to_string(transmitters.at(tx_id)->buffer.front()->get_id());
					transmitters.at(tx_id)->retransmission_packet = transmitters.at(tx_id)->buffer.front();
					transmitters.at(tx_id)->buffer.pop();
				}
				if (transmitters.at(tx_id)->is_retransmission_)
				{
					packet_id_s = std::to_string(transmitters.at(tx_id)->retransmission_packet->get_id());
				}
				logger.Error("Collision for TX " + tx_id_s + ", packet id: " +packet_id_s);
				++colissions_counter;
				is_transmission = false;
				
			}
		}
	}
}

int WirelessNetwork::GetBT(int tx_id)
{
	transmitters.at(tx_id)->CW = transmitters.at(tx_id)->CWmin;
	transmitters.at(tx_id)->BT = transmitters.at(tx_id)->BT_gen_->RandSection(0, transmitters.at(tx_id)->CW + 1);
	return transmitters.at(tx_id)->BT;
}

void WirelessNetwork::ChangeBT(Transmitter* transmitter)
{
	if (transmitter->CW == transmitter->CWmax)
	{
		logger.Error("PACKET LOST");
		lost_packets_counter += 1;
		transmitter->BT = -1;
		transmitter->retransmission_packet = nullptr;
		transmitter->is_retransmission_ = false;
	}
	else
	{
		transmitter->is_retransmission_ = true;
		transmitter->CW = std::min(2 * (transmitter->CW + 1) - 1, transmitter->CWmax);
		transmitter->BT = transmitter->BT_gen_->RandSection(0, transmitter->CW + 1);
		transmitter->DIFS = -2;
	}
}
