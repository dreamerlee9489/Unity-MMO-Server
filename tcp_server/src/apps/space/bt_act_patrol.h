#pragma once
#include "bt_action.h"

class BtActPatrol : public BtAction
{
public:
	BtActPatrol(Npc* npc);

	~BtActPatrol() = default;

	BtEventId GetEventId() override;

	void Enter() override;

	void Exit() override;

	void Broadcast() override;

	void Singlecast(Player* player) override;

private:
	int _index = 0;
	static std::default_random_engine _eng;
	static std::uniform_int_distribution<int> _dis;

	BtStatus PatrolTask();
};