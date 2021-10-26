#include "transmitter.h"



Transmitter::Transmitter(int identificator, Logger* logger, Generator* CGPk_gen, Generator* CTPk_gen, Generator* BT_gen) 
	: identificator_(identificator), logger_(logger), CGPk_gen_(CGPk_gen), CTPk_gen_(CTPk_gen), BT_gen_(BT_gen)
{
	std::string identificator_string = std::to_string(identificator_+1);					//Converting id to string
	logger_->Info("Transmitter " +identificator_string+ " and his generators have been created.");			//Send info about new Transmitter
}


Transmitter::Transmitter() {}
Transmitter::~Transmitter() {}

