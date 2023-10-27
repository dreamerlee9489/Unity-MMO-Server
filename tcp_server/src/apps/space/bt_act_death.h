#pragma once
#include "bt_action.h"

class BtActDeath : public BtAction
{
public:
	BtActDeath(Npc* npc);

	~BtActDeath() = default;

	BtEventId GetEventId() override;

	void Enter() override;

	void Exit() override {}

	BtStatus DeathTask();

	void Broadcast() override;

	void Singlecast(Player* player) override;
};