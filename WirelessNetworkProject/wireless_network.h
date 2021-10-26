#ifndef WIRELESS_NETWORK_H
#define WIRELESS_NETWORK_H
#include "event.h"
#include "logger.h"
#include "generate_packet.h"
#include "transmitter.h"
#include "channel.h"
#include "receiver.h"
#include "packet.h"
#include "event.h"
#include "generator.h"
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <fstream>


class WirelessNetwork : public Channel
{
public:
	WirelessNetwork();
	~WirelessNetwork();

	
	Logger logger = Logger();
	Receiver receiver = Receiver();

	void CreateNetwork(Event::EventList* event_list);
	void GeneratePacket(Transmitter* transmitter, Event::EventList* event_list, size_t time, Generator* generator);
	void BeginTransmission(Event::EventList* event_list, int tx_id, size_t clock);
	void EndTransmission(int tx_id);
	void CheckDIFS(size_t, Event::EventList* event_list);
	void SetACK();
	int GetBT(int tx_id);
	void StartListening(Event::EventList* event_list, size_t clock_, int tx_id);
	void fun_generate_timer(size_t time) { generate_timer = time; }
	size_t get_generate_time() { return generate_timer; }

	void CheckIfIsColission(int tx_id);
	void ChangeBT(Transmitter* transmitter);

	std::vector<Transmitter*> transmitters;
	std::vector<Receiver*> receivers;
	std::vector<Event*> events;
	std::vector<std::string> avg_retr_;

	Channel* channel_ = nullptr;

	int K = 35;
	bool ack = true;
	bool ACK_check_flag = true;
	size_t rand_time_transmission_ = 0;
	float lost_packets_counter = 0;
	int colissions_counter = 0;
	float sent_packets_counter = 0;
	float received_packets_counter = 0;
	float success_ratio_ = 0;
	bool is_transmission = false;
	int delayed_listenings_counter_ = 0;
	int retransmissions_counter = 0;
	int total_retransmissions_counter = 0;
	int TER_counter_ = 0;
	int retransmision_and_colission_counter = 0;
	int first_collission_counter = 0;
	int is_packet_waiting_for_retransmission_counter = 0;
	int is_now_transmission_counter = 0;
	float mean_packets_transmission_time = 0;
	double max_BER = 0;
	double avg_wait_time = 0;
	double avg_delay = 0;
	double bandwidth = 0;
	double avg_retr_number = 0;
	int initial_phase_ = 0;

	std::queue<int> kernels;
	std::string kernel_s;
	int kernel;
	int kernels_set_ = 0;


private:
	int time_ACK_ = 0;
	size_t generate_timer = -1;
	size_t transmission_timer = -1;
	double random;
	double L = 0.055;
	size_t id = 0;
	Packet* packet = nullptr;
	int transmission_number_ = 0;
	int current_tx_id = 0;
	int index = 0;
	double CGPk_ = 0;
	double P_ = 0.8;
	double TER_return_ = 0;
	Generator* TER_gen = nullptr;
	Generator* CGPk_gen = nullptr;
	Generator* CTPk_gen = nullptr;
	Generator* BT_gen = nullptr;
	std::ifstream read_kernel;
	
};

#endif WIRELESS_NETWORK_H	//	WIRELESS_NETWORK_H

