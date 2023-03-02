#ifndef BT_DECORATOR
#define BT_DECORATOR
#include "bt_node.h"

class BtDecorator : public BtNode
{
public:
	BtDecorator(Npc* npc, BtNode* child) : BtNode(npc), _child(child) {}

	virtual ~BtDecorator() = default;

	BtNode* GetChild() { return _child; }

	void HandleEvent(BtEventId id) override
	{
		if (callbacks.find(id) != callbacks.end())
			callbacks[id](id);
		else
			_child->HandleEvent(id);
	}

protected:
	BtNode* _child = nullptr;
};

#endif // !BT_DECORATOR
