#ifndef BT_ACTION_BIRTH
#define BT_ACTION_BIRTH
#include "bt_action.h"

class BtActBirth : public BtAction
{
public:
	BtActBirth(Npc* npc) : BtAction(npc)
	{
		_task = std::bind(&BtActBirth::BirthTask, this);
		funcMap.emplace(BtEventId::Alive, std::bind(&BtAction::SetSuccess, this, std::placeholders::_1));
	}

	~BtActBirth() = default;

	void Broadcast() override {}

	void Singlecast(Player* player) override {}

	void Enter() override {}

	void Exit() override {}

	BtStatus BirthTask()
	{
		return BtStatus::Running;
	}
};

#endif // !BT_ACTION_BIRTH
