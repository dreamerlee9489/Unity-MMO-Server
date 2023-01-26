#ifndef BT_REPEAT
#define BT_REPEAT
#include "bt_decorator.h"

class BtRepeat : public BtDecorator
{
public:
	BtRepeat(Npc* npc, BtNode* child, size_t limit = UINT_MAX) : BtDecorator(npc, child), _limit(limit) {}

	~BtRepeat() = default;

private:
	size_t _count = 0, _limit = UINT_MAX;

	void Enter() override {}

	BtStatus& Execute() override
	{
		while (_count < _limit)
		{
			BtStatus& tmp = _child->Tick();
			switch (tmp)
			{
			case BtStatus::Success:
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
};

#endif // !BT_REPEAT
