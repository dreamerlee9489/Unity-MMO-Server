#ifndef PVP_COMMAND_H
#define PVP_COMMAND_H
#include "command.h"

class PvpCommand : public Command
{
	Player* _target = nullptr;

public:
	PvpCommand(Player* owner, IEntity* target) : Command(owner)
	{
		_target = (Player*)target;
		target_sn = _target->GetPlayerSN();
	}

	void Enter() override;

	CmdState Execute() override;

	void Exit() override;

	void Broadcast() override;

	void Singlecast(Player* player) override;
};

#endif // !PVP_COMMAND_H
