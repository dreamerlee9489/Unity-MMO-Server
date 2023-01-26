#ifndef BT_ACTION
#define BT_ACTION
#include "bt_node.h"

class BtAction : public BtNode
{
public:
	using Task = std::function<BtStatus()>;

	BtAction(Npc* npc, Task task = nullptr) : BtNode(npc), _task(task) {}

	virtual ~BtAction() = default;

	virtual void Broadcast() = 0;

	virtual void Singlecast(Player* player) = 0;

	void SetSuccess(BtEventId id) { status = BtStatus::Success; }

	void SetFailure(BtEventId id) { status = BtStatus::Failure; }

	void HandleEvent(BtEventId id) override
	{
		if (funcMap.find(id) != funcMap.end())
			funcMap[id](id);
	}	

protected:
	Task _task = nullptr;	
	timeutil::Time _lastTime = 0, _currTime = 0, _timeElapsed = 0;

	BtStatus& Execute() override
	{
		if (_task)
			return status = _task();
		return status = BtStatus::Failure;
	}
};

#endif // !BT_ACTION
