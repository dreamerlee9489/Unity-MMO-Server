#ifndef BT_ACTION_SENSE
#define BT_ACTION_SENSE
#include "bt_action.h"

class BtActSense : public BtAction
{
public:
	BtActSense(Npc* npc) : BtAction(npc)
	{
		_task = std::bind(&BtActSense::SenseTask, this);
	}

	~BtActSense() = default;

	void Broadcast() override {}

	void Singlecast(Player* player) override {}

	void Enter() override {}

	void Exit() override {}

	BtStatus SenseTask()
	{
		return BtStatus::Running;
	}
};

#endif // !BT_ACTION_SENSE
