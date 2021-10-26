#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include "logger.h"
#include "channel.h"
#include "packet.h"
#include "generator.h"
#include <queue>

class Transmitter : public Channel
{
public:
	Transmitter(int identificator, Logger* logger, Generator* CGPk_gen, Generator* CTPk_gen, Generator* BT_gen);
	Transmitter();
	~Transmitter();
	
	std::queue<Packet*> buffer;
	Packet* retransmission_packet = nullptr;
	bool ACK_sent = false;
	bool is_listening = false;
	bool busy = false;
	bool blok_check = false;
	int GetTxId() { return identificator_;  }
	double DIFS = -1;
	bool BT_set = false;
	int CWmin = 15;
	int CWmax = 140;
	int CW = 0;
	int BT = -1;
	bool collision = false;
	size_t current_clock_ = -1;
	bool is_retransmission_ = false;
	int ACK_true_counter = 0;
	Generator* CGPk_gen_ = nullptr;
	Generator* CTPk_gen_ = nullptr;
	Generator* BT_gen_ = nullptr;

private:
	int identificator_ = 0;
	double random_ = 0;
	Logger* logger_ = nullptr;
	double generation_space_time = 0;
	double CGPk = 0;
	Channel* channel_;

};

#endif TRANSMITTER_H

