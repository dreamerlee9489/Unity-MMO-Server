#pragma once
#include "bt_action.h"

class BtActAttack : public BtAction
{
public:
	BtActAttack(Npc* npc);

	~BtActAttack() = default;

	BtEventId GetEventId() override;

	void Enter() override;

	void Exit() override;

	void Broadcast() override;

	void Singlecast(Player* player) override;

private:
	BtStatus AttackTask();
};
