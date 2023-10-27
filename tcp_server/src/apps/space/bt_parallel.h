#pragma once
#include "bt_composite.h"

enum struct BtPolicy { Sequence, Selector };

class BtParallel : public BtComposite
{
public:
	BtParallel(Npc* npc, BtPolicy policy = BtPolicy::Sequence) : BtComposite(npc), _policy(policy) {}

	~BtParallel() = default;

private:
	BtPolicy _policy = BtPolicy::Sequence;

	void Enter() override {}

	BtStatus& Execute() override
	{
		size_t succNum = 0, failNum = 0;
		auto iter = _children.begin();
		while (iter != _children.end())
		{
			BtNode* node = *iter++;
			switch (node->Tick())
			{
			case BtStatus::Success:
				++succNum;
				if (_policy == BtPolicy::Selector)
					return status = BtStatus::Success;
				break;
			case BtStatus::Failure:
				++failNum;
				if (_policy == BtPolicy::Sequence)
					return status = BtStatus::Failure;
				break;
			case BtStatus::Aborted:
				return status = BtStatus::Aborted;
			default:
				break;
			}
		}
		if (_policy == BtPolicy::Sequence && succNum == _children.size())
			return status = BtStatus::Success;
		if (_policy == BtPolicy::Selector && failNum == _children.size())
			return status = BtStatus::Failure;
		return status = BtStatus::Running;
	}
};