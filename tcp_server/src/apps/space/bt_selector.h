#ifndef BT_SELECTOR
#define BT_SELECTOR
#include "bt_composite.h"

class BtSelector : public BtComposite
{
public:
	BtSelector(Npc* npc) : BtComposite(npc) {}

	~BtSelector() = default;

private:
	void Enter() override { _curr = _children.begin(); }

	BtStatus& Execute() override
	{
		while (_curr != _children.end())
		{
			BtStatus& tmp = (*_curr)->Tick();
			switch (tmp)
			{
			case BtStatus::Failure:
				if (++_curr == _children.end())
					return status = BtStatus::Failure;
				break;
			case BtStatus::Aborted:
				if (++_curr == _children.end())
					return status = BtStatus::Aborted;
				break;
			default:
				return status = tmp;
			}
		}
		return status;
	}

	void Exit() override {}
};

#endif // !BT_SELECTOR
