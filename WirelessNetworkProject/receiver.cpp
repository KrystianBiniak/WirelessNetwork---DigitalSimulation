#include "receiver.h"


Receiver::Receiver(int identificator, Logger* logger) : identificator_(identificator), logger_(logger)
{
	std::string identificator_string = std::to_string(identificator_+1);		//Convert receiver's id to string
	logger_->Info("Receiver " + identificator_string + " created");			//Output that receiver [id] has been created
}

Receiver::Receiver() {}
