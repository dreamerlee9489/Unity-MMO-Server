#pragma once
#include "bt_action.h"

class BtActSense : public BtAction
{
public:
	BtActSense(Npc* npc);

	~BtActSense() = default;

	void Broadcast() override {}

	void Singlecast(Player* player) override {}

	void Enter() override {}

	void Exit() override {}

	BtStatus SenseTask();
};