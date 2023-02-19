#ifndef ATTACK_COMMAND_H
#define ATTACK_COMMAND_H
#include "command.h"

class AttackCommand : public Command
{
	Npc* _target = nullptr;

public:
	AttackCommand(Player* owner, IEntity* target) : Command(owner)
	{
		_target = (Npc*)target;
		target_sn = _target->GetSN();
	}

	void Enter() override;

	CmdState Execute() override;

	void Exit() override;

	void Broadcast() override;

	void Singlecast(Player* player) override;
};

#endif // !ATTACK_COMMAND_H
