#pragma once
#include "bt_action.h"

class BtActBirth : public BtAction
{
public:
	BtActBirth(Npc* npc);

	~BtActBirth() = default;

	BtEventId GetEventId() override;

	void Enter() override;

	void Exit() override {}

	BtStatus BirthTask();

	void Broadcast() override;

	void Singlecast(Player* player) override;
};