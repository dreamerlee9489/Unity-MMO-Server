#pragma once
#include "bt_action.h"

class BtActPursue : public BtAction
{
public:
	BtActPursue(Npc* npc);

	~BtActPursue() = default;

	BtEventId GetEventId() override;

	void Enter() override;

	void Exit() override;

	void Broadcast() override;

	void Singlecast(Player* player) override;

private:
	BtStatus PursueTask();
};