#ifndef BT_ACTION_VIEW
#define BT_ACTION_VIEW
#include "bt_action.h"

class BtActView : public BtAction
{
public:
	BtActView(Npc* npc) : BtAction(npc) 
	{
		_task = std::bind(&BtActView::ViewTask, this);
	}

	~BtActView() = default;

	void Broadcast() override {}

	void Singlecast(Player* player) override {}

	void Enter() override {}

	void Exit() override {}

	BtStatus ViewTask()
	{
		if (!_npc->target)
		{
			for (auto& pair : *_npc->GetAllPlayer())
			{
				if (_npc->CanSee(pair.second))
				{
					_npc->target = pair.second;
					//_npc->GetComponent<BtComponent>()->events.push(BtEvent(BtEventId::Pursue));
					break;
				}
			}
		}
		return BtStatus::Running;
	}
};

#endif // !BT_ACTION_VIEW
