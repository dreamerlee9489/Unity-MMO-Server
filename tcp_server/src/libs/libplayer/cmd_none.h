#ifndef NONE_COMMAND_H
#define NONE_COMMAND_H
#include "command.h"

class NoneCommand : public Command
{
public:
	NoneCommand(Player* owner) : Command(owner) {}

	void Broadcast() override;

	void Singlecast(Player* player) override;
};

#endif // !NONE_COMMAND_H

