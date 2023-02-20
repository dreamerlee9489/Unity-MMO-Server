#ifndef DEATH_COMMAND_H
#define DEATH_COMMAND_H
#include "command.h"

class DeathCommand : public Command
{
public:
	DeathCommand(Player* owner, uint64 target_sn) : Command(owner) 
	{
		this->target_sn = target_sn;
	}

	void Enter() override;

	CmdState Execute() override;

	void Exit() override;

	void Broadcast() override;

	void Singlecast(Player* player) override;
};

#endif // !DEATH_COMMAND_H
