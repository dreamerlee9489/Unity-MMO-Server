#ifndef BT_SELECTOR
#define BT_SELECTOR
#include "bt_composite.h"

class BtSelector : public BtComposite
{
public:
	BtSelector(Npc* npc) : BtComposite(npc) {}

	~BtSelector() = default;

	void AddChild(BtNode* child) override
	{
		BtComposite::AddChild(child);
		if (dynamic_cast<BtActBirth*>(child))
		{
			_nodeMap.emplace(BtEventId::Birth, std::prev(_children.end()));
			funcMap.emplace(BtEventId::Birth, std::bind(&BtSelector::SelectNode, this, std::placeholders::_1));
		}
		else if (dynamic_cast<BtActDeath*>(child))
		{
			_nodeMap.emplace(BtEventId::Death, std::prev(_children.end()));
			funcMap.emplace(BtEventId::Death, std::bind(&BtSelector::SelectNode, this, std::placeholders::_1));
		}
		else if (dynamic_cast<BtActIdle*>(child))
		{
			_nodeMap.emplace(BtEventId::Idle, std::prev(_children.end()));
			funcMap.emplace(BtEventId::Idle, std::bind(&BtSelector::SelectNode, this, std::placeholders::_1));
		}
		else if (dynamic_cast<BtActPatrol*>(child))
		{
			_nodeMap.emplace(BtEventId::Patrol, std::prev(_children.end()));
			funcMap.emplace(BtEventId::Patrol, std::bind(&BtSelector::SelectNode, this, std::placeholders::_1));
		}
		else if (dynamic_cast<BtActPursue*>(child))
		{
			_nodeMap.emplace(BtEventId::Pursue, std::prev(_children.end()));
			funcMap.emplace(BtEventId::Pursue, std::bind(&BtSelector::SelectNode, this, std::placeholders::_1));
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

	void SelectNode(BtEventId id) { _curr = _nodeMap[id]; }
};

#endif // !BT_SELECTOR
