#ifndef DIALOG_COMMAND_H
#define DIALOG_COMMAND_H
#include "command.h"

class ObserveCommand : public Command
{
	Player* _target = nullptr;

public:
	ObserveCommand(Player* owner, IEntity* target) : Command(owner)
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

#endif // !DIALOG_COMMAND_H
