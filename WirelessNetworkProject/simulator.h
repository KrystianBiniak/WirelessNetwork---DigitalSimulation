#ifndef SIMUALTOR_H_
#define SIMULATOR_H_
#include "wireless_network.h"
#include "check_ack.h"
#include "logger.h"
#include "channel.h"
#include <vector>
#include <string>

class Simulator
{
public:
	Simulator(WirelessNetwork* wireless_network);
	Simulator();

	bool ACK_check_flag = true;
	void Simulation(int duration);

private:
	size_t clock_ = 0;
	size_t clock_check_ = 0;
	WirelessNetwork* wireless_network_ = nullptr;
	bool pause_ = false;
	Transmitter* transmitter_ = nullptr;
	int mode_ = 0;
	bool work_ = true;
	int BT_ = 0;
	bool hold_ = false;
	int current_tx_id_ = 0;
	Logger logger = logger;
	double clock_show_ = 0;
	int event_type_;
	std::string tx_id_s;
	std::string BT_s;
};

#endif // !SIMUALTOR_H_
