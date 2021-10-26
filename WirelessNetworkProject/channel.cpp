#include "channel.h"

Channel::Channel(Logger* logger) : logger_(logger)
{
	logger_->Info("New channel created.");				//Output that channel has been created
}

Channel::Channel() {}
