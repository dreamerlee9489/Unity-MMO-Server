#pragma once
#include "bt_node.h"

class BtCondition : public BtNode
{
	using Task = std::function<bool()>;

public:
	BtCondition(Npc* npc, Task task) : BtNode(npc), _task(task) {}

	~BtCondition() = default;

	void HandleEvent(BtEventId id) override
	{
		if (callbacks.find(id) != callbacks.end())
			callbacks[id](id);
	}

private:
	Task _task = nullptr;

	void Enter() override {}

	BtStatus& Execute() override
	{
		if (_task && _task())
			return status = BtStatus::Success;
		return status = BtStatus::Failure;
	}

	void Exit() override {}
};