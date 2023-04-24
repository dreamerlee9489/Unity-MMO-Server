#ifndef BTED_SEQUENCE
#define BTED_SEQUENCE
#include "bt_composite.h"

class BtEdSequence : public BtComposite
{
public:
	BtEdSequence(Npc* npc) : BtComposite(npc) {}

	~BtEdSequence() = default;

	void AddChild(BtNode* child) override
	{
		BtComposite::AddChild(child);
		if (dynamic_cast<BtActIdle*>(child))
		{
			callbacks.emplace(BtEventId::Idle, std::bind(&BtEdSequence::SwitchNode, this, std::placeholders::_1));
		}
		else if (dynamic_cast<BtActPatrol*>(child))
		{
			callbacks.emplace(BtEventId::Patrol, std::bind(&BtEdSequence::SwitchNode, this, std::placeholders::_1));
		}
		else if (dynamic_cast<BtActPursue*>(child))
		{
			callbacks.emplace(BtEventId::Pursue, std::bind(&BtEdSequence::SwitchNode, this, std::placeholders::_1));
		}
		else if (dynamic_cast<BtActAttack*>(child))
		{
			callbacks.emplace(BtEventId::Attack, std::bind(&BtEdSequence::SwitchNode, this, std::placeholders::_1));
		}
		else if (dynamic_cast<BtActFlee*>(child))
		{
			callbacks.emplace(BtEventId::Flee, std::bind(&BtEdSequence::SwitchNode, this, std::placeholders::_1));
		}
	}

private:
	void Enter() override { _curr = _children.begin(); }

	BtStatus& Execute() override
	{
		if (_curr == _children.end())
			return status = BtStatus::Aborted;
		(*_curr)->Tick();
		return status = BtStatus::Running;
	}

	void Exit() override
	{
		if (_curr != _children.end())
			(*_curr)->ForceExit(BtStatus::Aborted);
	}

	void SwitchNode(BtEventId id)
	{
		(*_curr)->ForceExit(BtStatus::Suspend);
		++_curr;
	}
};

#endif // !BTED_SEQUENCE
