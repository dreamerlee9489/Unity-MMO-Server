#ifndef TELEPORT_COMMAND_H
#define TELEPORT_COMMAND_H
#include "command.h"

class TeleportCommand : public Command
{
public:
	TeleportCommand(Player* owner, uint64 target_sn) : Command(owner) 
	{
		this->target_sn = target_sn;
	}

	void Enter() override;

	CmdState Execute() override;

	void Exit() override;

	void Broadcast() override;

	void Singlecast(Player* player) override;
};

#endif // !TELEPORT_COMMAND_H
