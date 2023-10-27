#pragma once
#include "bt_action.h"

class BtActIdle : public BtAction
{
public:
	BtActIdle(Npc* npc);

	~BtActIdle() = default;

	BtEventId GetEventId() override;

	void Enter() override;

	void Exit() override;

	void Broadcast() override;

	void Singlecast(Player* player) override;

private:
	BtStatus IdleTask();
};