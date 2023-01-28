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
		if (_npc->hp == 0)
			return BtStatus::Aborted;	
		if (!_npc->fleeing && _npc->hp * 1.0 / _npc->initHp * 1.0 <= 0.3)
		{
			_npc->fleeing = true;
			_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Flee, 10);
		}
		return BtStatus::Running;
	}
};

#endif // !BT_ACTION_SENSE
