#ifndef BT_ED_SELECTOR
#define BT_ED_SELECTOR
#include "bt_composite.h"
#include "bt_act_idle.h"
#include "bt_act_patrol.h"
#include "bt_act_pursue.h"
#include "bt_act_attack.h"
#include "bt_act_flee.h"

class BtEdSelector : public BtComposite
{
public:
	BtEdSelector(Npc* npc) : BtComposite(npc) {}

	~BtEdSelector() = default;

	void AddChild(BtNode* child) override
	{
		BtComposite::AddChild(child);
		if (dynamic_cast<BtActIdle*>(child))
		{
			_nodeMap.emplace(BtEventId::Idle, std::prev(_children.end()));
			funcMap.emplace(BtEventId::Idle, std::bind(&BtEdSelector::SwitchNode, this, std::placeholders::_1));
		}
		else if (dynamic_cast<BtActPatrol*>(child))
		{
			_nodeMap.emplace(BtEventId::Patrol, std::prev(_children.end()));
			funcMap.emplace(BtEventId::Patrol, std::bind(&BtEdSelector::SwitchNode, this, std::placeholders::_1));
		}
		else if (dynamic_cast<BtActPursue*>(child))
		{
			_nodeMap.emplace(BtEventId::Pursue, std::prev(_children.end()));
			funcMap.emplace(BtEventId::Pursue, std::bind(&BtEdSelector::SwitchNode, this, std::placeholders::_1));
		}
		else if (dynamic_cast<BtActAttack*>(child))
		{
			_nodeMap.emplace(BtEventId::Attack, std::prev(_children.end()));
			funcMap.emplace(BtEventId::Attack, std::bind(&BtEdSelector::SwitchNode, this, std::placeholders::_1));
		}
		else if (dynamic_cast<BtActFlee*>(child))
		{
			_nodeMap.emplace(BtEventId::Flee, std::prev(_children.end()));
			funcMap.emplace(BtEventId::Flee, std::bind(&BtEdSelector::SwitchNode, this, std::placeholders::_1));
		}
	}

	void RmvChild(BtNode* child) override
	{
		BtComposite::RmvChild(child);
		for (auto iter = _nodeMap.begin(); iter != _nodeMap.end(); ++iter)
		{
			if (*(*iter).second == child)
			{
				_nodeMap.erase(iter);
				funcMap.erase((*iter).first);
				return;
			}
		}
	}

private:
	std::unordered_map<BtEventId, std::list<BtNode*>::iterator> _nodeMap;

	void Enter() override { _curr = _children.begin(); }

	BtStatus& Execute() override 
	{ 
		if (_curr == _children.end())
			return status = BtStatus::Aborted;
		status = (*_curr)->Tick(); 
		return status = BtStatus::Running;
	}

	void Exit() override { (*_curr)->ForceExit(BtStatus::Aborted); }

	void SwitchNode(BtEventId id) 
	{ 
		(*_curr)->ForceExit(BtStatus::Suspend);
		_curr = _nodeMap.find(id) == _nodeMap.end() ? _children.end() : _nodeMap[id];
	}
};

#endif // !BT_ED_SELECTOR
