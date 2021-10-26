#include "simulator.h"
#include "event.h"
#include "end_of_transmittion.h"
#include "channel_listener.h"
#include "generate_packet.h"
#include "check_ack.h"
#include <vector>
#include <fstream>
#include <iterator>


Simulator::Simulator(WirelessNetwork* wireless_netowrk) : wireless_network_(wireless_netowrk) {}

Simulator::Simulator() {}

void Simulator::Simulation(int duration) 
{
	auto cmp = [](Event* left, Event* right) { return left->get_time() > right->get_time(); };
	Event::EventList* event_list = new Event::EventList(cmp);
	logger.set_event_level(Logger::EventLevel::Info);
	clock_ = 0;
	wireless_network_->CreateNetwork(event_list);
	logger.Info("Start");
	logger.Info("Select mode: \n 1: step mode - press 1 \n 2: continuous mode - press 2");
	std::cin >> mode_;
	if (mode_ == 1) { work_ = false; }
	else work_ = true;

	while (clock_ < static_cast<size_t>(duration))
	{
		if(work_ == false)
		{
			logger.Info("---Press 1 to next step, press 2 to run continuously");
			std::cin >> mode_;
			if (mode_ == 2 ) { work_ = true; }
		}

		bool no_event = false;

		Event* event = event_list->top();
		event_type_ = event->GetEventType();
		event_list->pop();
		clock_ = event->get_time();
		clock_show_ = event->get_time();
		clock_show_ /= 10;
		std::string clock_s = std::to_string(clock_show_);
		Event* event_clock_check = event_list->top();
		clock_check_ = event_clock_check->get_time();
		if (event_type_ != 0) logger.Info("Simulation time: " + clock_s);
		event->Execute();
	
	//Channel listener sector
		if (event_type_ == 0) 
		{
			BT_s = std::to_string(event->transmitter_->BT - 1);
			wireless_network_->logger.set_event_level(Logger::EventLevel::Info);
			tx_id_s = std::to_string(event->transmitter_->GetTxId() + 1);
		}

		if (event_type_ == 0 && wireless_network_->channel_->is_free && !event->transmitter_->buffer.empty() && !wireless_network_->is_transmission)
		{
			std::string DIFS_s = std::to_string(event->transmitter_->DIFS+1);
			std::string clock_s = std::to_string(clock_);
			event->transmitter_->current_clock_ = clock_;
			event->transmitter_->DIFS += 1;
		}
		if (mode_ == 1 && event_type_ == 0 && wireless_network_->channel_->is_free && !event->transmitter_->buffer.empty() && !wireless_network_->is_transmission)
			wireless_network_->logger.Info("Tx " + tx_id_s + " is listening.");

		if (mode_ == 1 && event_type_ == 0 && wireless_network_->channel_->is_free && !event->transmitter_->buffer.empty() && !wireless_network_->is_transmission
			&& event->transmitter_->BT == 0 && wireless_network_->channel_->is_free)
			wireless_network_->logger.Info("BT for Tx " + tx_id_s + " is " + BT_s);

		if (mode_ == 1 && event_type_ == 0 && wireless_network_->channel_->is_free && !event->transmitter_->buffer.empty() && !wireless_network_->is_transmission
			&& event->transmitter_->BT < 0 && wireless_network_->channel_->is_free)
			wireless_network_->logger.Info("BT for Tx " + tx_id_s + " is not set yet.");
			
		if (event_type_ == 0 && wireless_network_->channel_->is_free && !event->transmitter_->buffer.empty() && !wireless_network_->is_transmission
			&& event->transmitter_->BT > 0 && wireless_network_->channel_->is_free)
			event->transmitter_->BT -= 1;

		if (mode_ == 1 && event_type_ == 0 && wireless_network_->channel_->is_free && !event->transmitter_->buffer.empty() && !wireless_network_->is_transmission
			&& event->transmitter_->BT > 0 && wireless_network_->channel_->is_free)
				wireless_network_->logger.Info("BT for Tx " + tx_id_s + " is " + BT_s);

		if (event_type_ == 0 && !wireless_network_->channel_->is_free && !event->transmitter_->buffer.empty() && wireless_network_->is_transmission == true
			&& !event->transmitter_->is_listening && wireless_network_->delayed_listenings_counter_ < 3 && event->transmitter_->ACK_sent == false)
		{
			wireless_network_->StartListening(event_list, wireless_network_->rand_time_transmission_ - 5, event->transmitter_->GetTxId());
			event->transmitter_->is_listening = true;
			wireless_network_->delayed_listenings_counter_++;
		}
		if (event_type_ == 0 && event->transmitter_->ACK_sent == true && event->transmitter_->is_listening)
		{
			wireless_network_->StartListening(event_list, wireless_network_->rand_time_transmission_ - 5, event->transmitter_->GetTxId());
			event->transmitter_->is_listening = true;
		}
	//END OF Channel listner sector

	//Check ACK sector

		if (event_type_ == 2 && event->transmitter_->GetTxId() != wireless_network_->channel_->channel_buffer.front()->GetPacketsTxId())
		{
			std::swap(wireless_network_->channel_->channel_buffer.front(), wireless_network_->channel_->channel_buffer.back());
		}

		if ( event_type_ == 2 && wireless_network_->ack == true && wireless_network_->channel_->channel_buffer.front() != nullptr
			&& !wireless_network_->channel_->channel_buffer.front()->delivered && !event->transmitter_->collision && event->transmitter_->BT < 0)
		{
			wireless_network_->avg_delay += (clock_ - wireless_network_->channel_->channel_buffer.front()->generation_time) / 10;
			event->transmitter_->ACK_true_counter += 1;
			wireless_network_->channel_->channel_buffer.front()->delivered = true;
			std::string packet_id_s = std::to_string(wireless_network_->channel_->channel_buffer.front()->get_id());
			logger.Info("Previous transmission passed, ACK received. Packet id: " + packet_id_s);	
			wireless_network_->received_packets_counter += 1;
			wireless_network_->avg_retr_.push_back(std::to_string((wireless_network_->total_retransmissions_counter / wireless_network_->sent_packets_counter)));	//Collecting data
			wireless_network_->mean_packets_transmission_time += (event->get_time() - wireless_network_->channel_->channel_buffer.front()->start_clock_);
			event->transmitter_->CW = 0;
			event->transmitter_->BT = -1;
			event->transmitter_->is_retransmission_ = false;
			wireless_network_->receivers.at(event->transmitter_->GetTxId())->packets_received.push(wireless_network_->channel_->channel_buffer.front());
			wireless_network_->channel_->channel_buffer.pop();
			event->transmitter_->retransmission_packet = nullptr;
		}

		if (event_type_ == 2 && wireless_network_->ack == false && wireless_network_->channel_->channel_buffer.front() != nullptr) 
		{
			std::string packet_id_s = std::to_string(wireless_network_->channel_->channel_buffer.front()->get_id());
			logger.Info("Previous transmission failed, TER Error occurred. Packet id: " + packet_id_s);	
			wireless_network_->ChangeBT(event->transmitter_);
		}

		if (event_type_ == 2 && wireless_network_->ack == false && wireless_network_->channel_->channel_buffer.front() != nullptr && event->transmitter_->BT != -1)
			event->transmitter_->retransmission_packet = wireless_network_->channel_->channel_buffer.front();

		if (event_type_ == 2 && wireless_network_->ack == false && wireless_network_->channel_->channel_buffer.front() != nullptr)
		{
			wireless_network_->channel_->channel_buffer.pop();
			wireless_network_->TER_counter_ += 1;
		}

		if(event_type_ == 2) wireless_network_->ACK_check_flag = true;

		if(event_type_ == 2 && !wireless_network_->is_transmission) wireless_network_->channel_->is_free = true;
		
	//END OF Check ACK sector
		
		hold_ = true;

		for (int i = 0 ; i < wireless_network_->K ; ++i)	//Pêtla przeszukuj¹ca bufory transmitterów, ustaw flagê transmisji(hold_) zale¿nie od tego, czy w buforach nadal s¹ pakiety
		{
			if (wireless_network_->transmitters.at(i)->buffer.empty()) hold_ = true;
			else
			{
				hold_ = false;
				break;
			}
		}
		for (int i = 0; i < wireless_network_->K; ++i)
		{
			if (wireless_network_->channel_->is_free && wireless_network_->transmitters.at(i)->BT == 0
				&& !wireless_network_->transmitters.at(i)->buffer.empty() && wireless_network_->transmitters.at(i)->collision == false
				&& clock_ + 5 >= clock_check_)
			{
				wireless_network_->CheckIfIsColission(i);
			}
		}
		while (no_event == false)
		{
			no_event = true;
			{
				for (int i = 0; i < wireless_network_->K; ++i)
				{
					if (wireless_network_->transmitters.at(i)->collision && wireless_network_->transmitters.at(i)->is_retransmission_)
						wireless_network_->retransmision_and_colission_counter += 1;

					if (wireless_network_->transmitters.at(i)->collision && !wireless_network_->transmitters.at(i)->is_retransmission_)
					{
						wireless_network_->sent_packets_counter += 1;
						wireless_network_->initial_phase_ += 1;
						wireless_network_->first_collission_counter += 1;
					}
					//INITIAL PHASE INCLUDED
					if (wireless_network_->initial_phase_ == 250 && wireless_network_->sent_packets_counter == 250)
					{
						wireless_network_->sent_packets_counter = 0;
						wireless_network_->received_packets_counter = 0;
						wireless_network_->lost_packets_counter = 0;
						wireless_network_->is_packet_waiting_for_retransmission_counter = 0;
						wireless_network_->mean_packets_transmission_time = 0;
						wireless_network_->retransmissions_counter = 0;
						wireless_network_->total_retransmissions_counter = 0;
						wireless_network_->colissions_counter = 0;
						wireless_network_->TER_counter_ = 0;
						wireless_network_->first_collission_counter = 0;
						wireless_network_->retransmision_and_colission_counter = 0;
					}
					if (wireless_network_->transmitters.at(i)->collision)
					{
						wireless_network_->transmitters.at(i)->busy = true;
						wireless_network_->transmitters.at(i)->collision = false;
						wireless_network_->ChangeBT(wireless_network_->transmitters.at(i));
						no_event = false;
					}
				}
				for (int i = 0; i < wireless_network_->K; ++i)
				{
					if (!wireless_network_->transmitters.at(i)->buffer.empty() && wireless_network_->channel_->is_free &&
						wireless_network_->transmitters.at(i)->DIFS < 1 && wireless_network_->transmitters.at(i)->BT < 0 &&
						clock_ != clock_check_ && !wireless_network_->transmitters.at(i)->is_listening && !wireless_network_->transmitters.at(i)->ACK_sent)		//SprawdŸ, czy kana³ jest wolny i czy znajduje siê jakikolwiek pakiet w jakimkolwiek buforze transmittera	
					{
						wireless_network_->StartListening(event_list, clock_, i);
						wireless_network_->transmitters.at(i)->is_listening = true;
						no_event = false;
					}
				}
				for (int i = 0; i < wireless_network_->K; ++i)
				{
					if (wireless_network_->transmitters.at(i)->DIFS == 1)
					{
						wireless_network_->transmitters.at(i)->DIFS = -1;
						wireless_network_->transmitters.at(i)->busy = true;
						BT_ = wireless_network_->GetBT(i);
						no_event = false;
					}
				}
				for (int i = 0; i < wireless_network_->K; ++i)
				{
					if (wireless_network_->channel_->is_free && wireless_network_->transmitters.at(i)->BT > 0 && wireless_network_->transmitters.at(i)->DIFS <= -1
						&& !wireless_network_->transmitters.at(i)->is_listening)
					{
						wireless_network_->transmitters.at(i)->busy = false;
						wireless_network_->StartListening(event_list, clock_, i);
						wireless_network_->transmitters.at(i)->busy = true;
						wireless_network_->transmitters.at(i)->DIFS = -2;
						wireless_network_->transmitters.at(i)->is_listening = true;
						no_event = false;
					}
				}
				for (int i = 0; i < wireless_network_->K; ++i)
				{
					if (wireless_network_->channel_->is_free && wireless_network_->transmitters.at(i)->BT == 0
						&& !wireless_network_->transmitters.at(i)->buffer.empty() && wireless_network_->transmitters.at(i)->collision == false
						&& (clock_ + 5 == clock_check_ ))
					{
						wireless_network_->BeginTransmission(event_list, i, clock_);
						no_event = false;
					}
				}
			}
		}
		if (mode_ == 1) { work_ = false;  }

		//COLLECTING DATA
		if ( wireless_network_->initial_phase_ >= 350 
			&& (1 - (wireless_network_->received_packets_counter
			/ (wireless_network_->sent_packets_counter - wireless_network_->is_now_transmission_counter - wireless_network_->is_packet_waiting_for_retransmission_counter))
				> wireless_network_->max_BER))
			wireless_network_->max_BER = 1 - (wireless_network_->received_packets_counter
				/ (wireless_network_->sent_packets_counter - wireless_network_->is_now_transmission_counter - wireless_network_->is_packet_waiting_for_retransmission_counter));
	}
	for (int i = 0; i < wireless_network_->K; ++i)
	{
		if (wireless_network_->transmitters.at(i)->retransmission_packet != nullptr)
		{
			++wireless_network_->is_packet_waiting_for_retransmission_counter;
		}
		if (!wireless_network_->channel_->channel_buffer.empty() && wireless_network_->channel_->channel_buffer.front()->GetPacketsTxId() == i)
		{
			++wireless_network_->is_now_transmission_counter;
		}
	}

	std::ofstream write_avg_retr("./avg_retr.txt");
	std::ostream_iterator<std::string> output_iterator(write_avg_retr, ", ");
	std::copy(wireless_network_->avg_retr_.begin(), wireless_network_->avg_retr_.end(), output_iterator);

	wireless_network_->bandwidth = wireless_network_->received_packets_counter / clock_show_ * 1000;
	wireless_network_->mean_packets_transmission_time /= wireless_network_->sent_packets_counter;
	wireless_network_->avg_retr_number = wireless_network_->total_retransmissions_counter / wireless_network_->sent_packets_counter;
	wireless_network_->avg_delay /= wireless_network_->received_packets_counter;
	wireless_network_->avg_wait_time /= (wireless_network_->sent_packets_counter - wireless_network_->first_collission_counter);
	wireless_network_->success_ratio_ = 1 - (wireless_network_->received_packets_counter / (wireless_network_->sent_packets_counter - wireless_network_->is_now_transmission_counter - wireless_network_->is_packet_waiting_for_retransmission_counter));


	std::cout << "\nSent packets: " << wireless_network_->sent_packets_counter << "\n";
	std::cout << "Whole phase: " << wireless_network_->initial_phase_ << "\n";
	std::cout << "Packets waiting for retransmission: " << wireless_network_->is_packet_waiting_for_retransmission_counter << "\n";
	std::cout << "Packets now transmitted: " << wireless_network_->is_now_transmission_counter << "\n";
	std::cout << "Received packets: " << wireless_network_->received_packets_counter << "\n";
	std::cout << "Retransmitted packets: " << wireless_network_->retransmissions_counter << "\n";
	std::cout << "Total retransmissions: " << wireless_network_->total_retransmissions_counter << "\n";
	std::cout << "PER: " << wireless_network_->success_ratio_ * 100 << "%\n";
	std::cout << "Total collisions: " << (wireless_network_->colissions_counter / 2) -1 << "\n";
	std::cout << "TER counter: " << wireless_network_->TER_counter_ << "\n";
	std::cout << "Collissions during first transmission: " << wireless_network_->first_collission_counter / 2 << "\n";
	std::cout << "Collissions during retransmissions: " << wireless_network_->retransmision_and_colission_counter / 2 << "\n";
	std::cout << "Max PER: " << wireless_network_->max_BER *100 << "%\n";
	std::cout << "Average retransmissions number: " << wireless_network_->avg_retr_number << "\n";
	std::cout << "Waiting time: " << wireless_network_->avg_wait_time << "ms\n";
	std::cout << "Average delay: " << wireless_network_->avg_delay << "ms\n";
	std::cout << "Bandwidth: " << wireless_network_->bandwidth << " received packets/s\n";

	wireless_network_->initial_phase_ = 0;
	wireless_network_->sent_packets_counter = 0;
}