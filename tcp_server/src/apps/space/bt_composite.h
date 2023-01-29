#ifndef BT_COMPOSITE
#define BT_COMPOSITE
#include "bt_node.h"
#include <list>
#include <unordered_map>

class BtComposite : public BtNode
{
public:
	BtComposite(Npc* npc) : BtNode(npc) {}

	virtual ~BtComposite() = default;

	void Reset() override
	{
		Exit();
		_curr = _children.begin();
		status = BtStatus::Invalid;
	}

	void Exit() override
	{
		for (auto iter = _children.begin(); iter != _children.end(); ++iter)
			(*iter)->ForceExit(BtStatus::Invalid);
	}

	virtual void AddChild(BtNode* child) { _children.push_back(child); }

	virtual void RmvChild(BtNode* child) { _children.remove(child); }

	void HandleEvent(BtEventId id) override
	{
		if (funcMap.find(id) != funcMap.end())
			funcMap[id](id);
		else
		{
			for (BtNode* node : _children)
				node->HandleEvent(id);
		}
	}

protected:
	std::list<BtNode*> _children;
	std::list<BtNode*>::iterator _curr = _children.begin();	
};

#endif // !BT_COMPOSITE

