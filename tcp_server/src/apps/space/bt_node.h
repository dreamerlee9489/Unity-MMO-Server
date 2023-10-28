#pragma once
#include "libplayer/player.h"
#include <list>
#include <unordered_map>
#include <initializer_list>
#include <functional>

class Npc;
class Player;
class BtComponent;
enum struct BtEventId { Unknow, Birth, Alive, Death, Idle, Patrol, Pursue, Attack, Flee };
enum struct BtStatus { Invalid, Running, Success, Failure, Suspend, Aborted };
enum struct BtPolicy { Sequence, Selector };

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
	std::unordered_map<BtEventId, std::function<void(BtEventId)>> callbacks;

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

class BtAction : public BtNode
{
public:
	using Task = std::function<BtStatus()>;

	BtAction(Npc* npc, Task task = nullptr) : BtNode(npc), _task(task) {}

	virtual ~BtAction() = default;

	virtual void Broadcast() = 0;

	virtual void Singlecast(Player* player) = 0;

	virtual BtEventId GetEventId()
	{
		return BtEventId::Unknow;
	}

	void HandleEvent(BtEventId id) override
	{
		if (callbacks.find(id) != callbacks.end())
			callbacks[id](id);
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

class BtActAttack : public BtAction
{
public:
	BtActAttack(Npc* npc) : BtAction(npc)
	{
		_task = std::bind(&BtActAttack::AttackTask, this);
	}

	~BtActAttack() = default;

	BtEventId GetEventId() override { return BtEventId::Attack; }

	void Enter() override;

	void Exit() override;

	void Broadcast() override;

	void Singlecast(Player* player) override;

private:
	BtStatus AttackTask();
};

class BtActBirth : public BtAction
{
public:
	BtActBirth(Npc* npc) : BtAction(npc)
	{
		_task = std::bind(&BtActBirth::BirthTask, this);
	}

	~BtActBirth() = default;

	BtEventId GetEventId() override
	{
		return BtEventId::Birth;
	}

	void Enter() override;

	void Exit() override {}

	void Broadcast() override;

	void Singlecast(Player* player) override;

private:
	BtStatus BirthTask();
};

class BtActDeath : public BtAction
{
public:
	BtActDeath(Npc* npc) : BtAction(npc)
	{
		_task = std::bind(&BtActDeath::DeathTask, this);
	}

	~BtActDeath() = default;

	BtEventId GetEventId() override
	{
		return BtEventId::Birth;
	}

	void Enter() override;

	void Exit() override {}

	void Broadcast() override;

	void Singlecast(Player* player) override;

private:
	BtStatus DeathTask();
};

class BtActFlee : public BtAction
{
public:
	BtActFlee(Npc* npc) : BtAction(npc)
	{
		_task = std::bind(&BtActFlee::FleeTask, this);
	}

	~BtActFlee() = default;

	BtEventId GetEventId() override
	{
		return BtEventId::Flee;
	}

	void Enter() override;

	void Exit() override;

	void Broadcast() override;

	void Singlecast(Player* player) override;

private:
	BtStatus FleeTask();
};

class BtActIdle : public BtAction
{
public:
	BtActIdle(Npc* npc) : BtAction(npc)
	{
		_task = std::bind(&BtActIdle::IdleTask, this);
	}

	~BtActIdle() = default;

	BtEventId GetEventId() override
	{
		return BtEventId::Idle;
	}

	void Enter() override;

	void Exit() override;

	void Broadcast() override;

	void Singlecast(Player* player) override;

private:
	BtStatus IdleTask();
};

class BtActPatrol : public BtAction
{
public:
	BtActPatrol(Npc* npc);

	~BtActPatrol() = default;

	BtEventId GetEventId() override
	{
		return BtEventId::Patrol;
	}

	void Enter() override;

	void Exit() override;

	void Broadcast() override;

	void Singlecast(Player* player) override;

private:
	int _index = 0;
	static std::default_random_engine _eng;
	static std::uniform_int_distribution<int> _dis;

	BtStatus PatrolTask();
};

class BtActPursue : public BtAction
{
public:
	BtActPursue(Npc* npc) : BtAction(npc)
	{
		_task = std::bind(&BtActPursue::PursueTask, this);
	}

	~BtActPursue() = default;

	BtEventId GetEventId() override
	{
		return BtEventId::Pursue;
	}

	void Enter() override;

	void Exit() override;

	void Broadcast() override;

	void Singlecast(Player* player) override;

private:
	BtStatus PursueTask();
};

class BtActSense : public BtAction
{
public:
	BtActSense(Npc* npc) : BtAction(npc)
	{
		_task = std::bind(&BtActSense::SenseTask, this);
	}

	~BtActSense() = default;

	void Broadcast() override {}

	void Singlecast(Player* player) override {}

	void Enter() override {}

	void Exit() override {}

	BtStatus SenseTask();
};

class BtActView : public BtAction
{
public:
	BtActView(Npc* npc) : BtAction(npc)
	{
		_task = std::bind(&BtActView::ViewTask, this);
	}

	~BtActView() = default;

	void Broadcast() override {}

	void Singlecast(Player* player) override {}

	void Enter() override {}

	void Exit() override {}

	BtStatus ViewTask();
};

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

class BtComposite : public BtNode
{
public:
	BtComposite(Npc* npc) : BtNode(npc)
	{
		_curr = _children.begin();
	}

	virtual ~BtComposite() = default;

	virtual void AddChild(BtNode* child)
	{
		_children.push_back(child);
	}

	virtual void RmvChild(BtNode* child)
	{
		_children.remove(child);
	}

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

	void AddChildren(std::initializer_list<BtNode*> children)
	{
		for (auto& child : children)
			AddChild(child);
	}

	void HandleEvent(BtEventId id) override
	{
		if (callbacks.find(id) != callbacks.end())
			callbacks[id](id);
		else
		{
			for (BtNode* node : _children)
				node->HandleEvent(id);
		}
	}

protected:
	std::list<BtNode*> _children;
	std::list<BtNode*>::iterator _curr;
};

class BtSelector : public BtComposite
{
public:
	BtSelector(Npc* npc) : BtComposite(npc) {}

	~BtSelector() = default;

private:
	void Enter() override { _curr = _children.begin(); }

	BtStatus& Execute() override
	{
		while (_curr != _children.end())
		{
			BtStatus& tmp = (*_curr)->Tick();
			switch (tmp)
			{
			case BtStatus::Failure:
			case BtStatus::Aborted:
				if (++_curr == _children.end())
					return status = BtStatus::Failure;
				break;
			default:
				return status = tmp;
			}
		}
		return status;
	}
};

class BtSequence : public BtComposite
{
public:
	BtSequence(Npc* npc) : BtComposite(npc) {}

	~BtSequence() = default;

private:
	void Enter() override { _curr = _children.begin(); }

	BtStatus& Execute() override
	{
		while (_curr != _children.end())
		{
			BtStatus& tmp = (*_curr)->Tick();
			switch (tmp)
			{
			case BtStatus::Success:
			case BtStatus::Aborted:
				if (++_curr == _children.end())
					return status = BtStatus::Success;
				break;
			default:
				return status = tmp;
			}
		}
		return status;
	}
};

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
			_nodes.emplace(BtEventId::Idle, std::prev(_children.end()));
			callbacks.emplace(BtEventId::Idle, std::bind(&BtEdSelector::SwitchNode, this, std::placeholders::_1));
		}
		else if (dynamic_cast<BtActPatrol*>(child))
		{
			_nodes.emplace(BtEventId::Patrol, std::prev(_children.end()));
			callbacks.emplace(BtEventId::Patrol, std::bind(&BtEdSelector::SwitchNode, this, std::placeholders::_1));
		}
		else if (dynamic_cast<BtActPursue*>(child))
		{
			_nodes.emplace(BtEventId::Pursue, std::prev(_children.end()));
			callbacks.emplace(BtEventId::Pursue, std::bind(&BtEdSelector::SwitchNode, this, std::placeholders::_1));
		}
		else if (dynamic_cast<BtActAttack*>(child))
		{
			_nodes.emplace(BtEventId::Attack, std::prev(_children.end()));
			callbacks.emplace(BtEventId::Attack, std::bind(&BtEdSelector::SwitchNode, this, std::placeholders::_1));
		}
		else if (dynamic_cast<BtActFlee*>(child))
		{
			_nodes.emplace(BtEventId::Flee, std::prev(_children.end()));
			callbacks.emplace(BtEventId::Flee, std::bind(&BtEdSelector::SwitchNode, this, std::placeholders::_1));
		}
	}

	void RmvChild(BtNode* child) override
	{
		BtComposite::RmvChild(child);
		for (auto iter = _nodes.begin(); iter != _nodes.end(); ++iter)
		{
			if (*(*iter).second == child)
			{
				_nodes.erase(iter);
				callbacks.erase((*iter).first);
				return;
			}
		}
	}

private:
	std::unordered_map<BtEventId, std::list<BtNode*>::iterator> _nodes;

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
		_curr = _nodes.find(id) == _nodes.end() ? _children.end() : _nodes[id];
	}
};

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

class BtRepeat : public BtDecorator
{
public:
	BtRepeat(Npc* npc, BtNode* child, size_t limit = UINT_MAX) : BtDecorator(npc, child), _limit(limit)
	{
		callbacks.emplace(BtEventId::Birth, std::bind(&BtRepeat::HandleRebirth, this, std::placeholders::_1));
	}

	~BtRepeat() = default;

private:
	size_t _count = 0, _limit = UINT_MAX;

	void Enter() override {}

	BtStatus& Execute() override
	{
		if (_count < _limit)
		{
			BtStatus& tmp = _child->Tick();
			switch (tmp)
			{
			case BtStatus::Success:
				_child->Reset();
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

	void HandleRebirth(BtEventId id)
	{
		_count = 0;
		_child->Reset();
	}
};