#ifndef BT_ACTION_DEATH
#define BT_ACTION_DEATH
#include "bt_action.h"

class BtActDeath : public BtAction
{
public:
	BtActDeath(Npc* npc) : BtAction(npc)
	{
		_task = std::bind(&BtActDeath::DeathTask, this);
	}

	~BtActDeath() = default;

	void Broadcast() override {}

	void Singlecast(Player* player) override {}

	void Enter() override {}

	void Exit() override {}

	BtStatus DeathTask()
	{
		return BtStatus::Running;
	}
};

#endif // !BT_ACTION_DEATH
