#ifndef NONE_COMMAND_H
#define NONE_COMMAND_H
#include "command.h"

class NoneCommand : public Command
{
public:
	NoneCommand(Player* owner) : Command(owner) {}

	void Enter() override;

	CmdState Execute() override;

	void Exit() override;

	void Broadcast() override;

	void Singlecast(Player* player) override;
};

#endif // !NONE_COMMAND_H

