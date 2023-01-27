#ifndef BT_ACTION_BIRTH
#define BT_ACTION_BIRTH
#include "bt_action.h"

class BtActBirth : public BtAction
{
public:
	BtActBirth(Npc* npc) : BtAction(npc)
	{
		_task = std::bind(&BtActBirth::BirthTask, this);
	}

	~BtActBirth() = default;

	void Broadcast() override {}

	void Singlecast(Player* player) override {}

	void Enter() override 
	{
		_currTime = _lastTime = Global::GetInstance()->TimeTick;
	}

	void Exit() override {}

	BtStatus BirthTask()
	{
		_currTime = Global::GetInstance()->TimeTick;
		_timeElapsed = _currTime - _lastTime;
		if (_timeElapsed < 2000)
			return BtStatus::Running;
		return BtStatus::Success;
	}
};

#endif // !BT_ACTION_BIRTH
