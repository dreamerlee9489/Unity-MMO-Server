#include "bt_decorator.h"

class BtRepeat : public BtDecorator
{
public:
	BtRepeat(Npc* npc, BtNode* child, size_t limit = UINT_MAX) : BtDecorator(npc, child), _limit(limit) 
	{
		callbacks.emplace(BtEventId::Birth, std::bind(&BtRepeat::HandleRebirth, this, std::placeholders::_1));
	}

	~BtRepeat() = default;

private:
	size_t _count = 0, _limit = UINT_MAX;

	void Enter() override {}

	BtStatus& Execute() override
	{
		if (_count < _limit)
		{
			BtStatus& tmp = _child->Tick();
			switch (tmp)
			{
			case BtStatus::Success:
				_child->Reset();
				if (++_count == _limit)
					return status = BtStatus::Success;
				break;
			default:
				return status = tmp;
			}
		}
		return status;
	}

	void Exit() override {}

	void HandleRebirth(BtEventId id)
	{		
		_count = 0;
		_child->Reset();
	}
};