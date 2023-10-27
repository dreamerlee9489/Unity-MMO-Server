#pragma once
#include "bt_action.h"

class BtActFlee : public BtAction
{
public:
	BtActFlee(Npc* npc);

	~BtActFlee() = default;

	BtEventId GetEventId() override;

	void Enter() override;

	void Exit() override;

	void Broadcast() override;

	void Singlecast(Player* player) override;

private:
	BtStatus FleeTask();
};