#pragma once
#include "bt_action.h"

class BtActView : public BtAction
{
public:
	BtActView(Npc* npc);

	~BtActView() = default;

	void Broadcast() override {}

	void Singlecast(Player* player) override {}

	void Enter() override {}

	void Exit() override {}

	BtStatus ViewTask();
};