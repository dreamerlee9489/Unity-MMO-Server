#ifndef BT_NODE
#define BT_NODE
#include "libplayer/player.h"
#include "npc.h"
#include <functional>
#include <unordered_map>

class Npc;
class Player;
enum struct BtEventId { Birth, Alive, Death, Idle, Patrol, Pursue, Attack, Flee };
enum struct BtStatus { Invalid, Running, Success, Failure, Suspend, Aborted };

struct BtEvent
{
	BtEventId id = BtEventId::Birth;
	int priority = 0;

	friend bool operator< (const BtEvent& lhs, const BtEvent& rhs) { return lhs.priority < rhs.priority; }
	friend bool operator==(const BtEvent& lhs, const BtEvent& rhs) { return lhs.id == rhs.id; }
	friend bool operator!=(const BtEvent& lhs, const BtEvent& rhs) { return lhs.id != rhs.id; }

	BtEvent(BtEventId id, int priority = 0)
	{
		this->id = id;
		this->priority = priority;
	}
};

class BtNode
{
public:
	BtStatus status = BtStatus::Invalid;
	std::unordered_map<BtEventId, std::function<void(BtEventId)>> funcMap;

	BtNode(Npc* npc) : _npc(npc) {}

	virtual ~BtNode() = default;

	virtual void HandleEvent(BtEventId id) = 0;

	virtual void Reset() { status = BtStatus::Invalid; }

	Npc* GetNpc() { return _npc; }

	BtStatus& Tick()
	{		
		if (status != BtStatus::Success && status != BtStatus::Failure)
		{
			if (status != BtStatus::Running)
				Enter();
			status = Execute();
			if (status != BtStatus::Running)
				Exit();
		}
		return status;
	}

	void ForceExit(BtStatus status = BtStatus::Invalid)
	{
		this->status = status;
		Exit();
	}

protected:
	Npc* _npc = nullptr;

	virtual void Enter() = 0;

	virtual BtStatus& Execute() = 0;

	virtual void Exit() = 0;
};

#endif // !BT_NODE
