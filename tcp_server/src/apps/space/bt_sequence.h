#pragma once
#include "bt_composite.h"

class BtSequence : public BtComposite
{
public:
	BtSequence(Npc* npc) : BtComposite(npc) {}

	~BtSequence() = default;

private:
	void Enter() override { _curr = _children.begin(); }

	BtStatus& Execute() override
	{
		while (_curr != _children.end())
		{
			BtStatus& tmp = (*_curr)->Tick();
			switch (tmp)
			{
			case BtStatus::Success:
			case BtStatus::Aborted:
				if (++_curr == _children.end())
					return status = BtStatus::Success;
				break;
			default:
				return status = tmp;
			}
		}
		return status;
	}
};