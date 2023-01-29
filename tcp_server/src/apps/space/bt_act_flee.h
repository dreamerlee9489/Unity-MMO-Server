#ifndef BT_ACTION_FLEE
#define BT_ACTION_FLEE
#include "bt_action.h"

class BtActFlee : public BtAction
{
public:
	BtActFlee(Npc* npc) : BtAction(npc)
	{
		_task = std::bind(&BtActFlee::FleeTask, this);
	}

	~BtActFlee() = default;

	BtEventId GetEventId() override { return BtEventId::Flee; }

	void Enter() override
	{		
		_npc->SetNextPos(_npc->GetInitPos());
		_lastTime = _currTime = Global::GetInstance()->TimeTick;
		_npc->GetComponent<BtComponent>()->curAct = this;
		Broadcast();
	}

	void Exit() override
	{
		_npc->target = nullptr;
		_npc->fleeing = false;
		_npc->GetComponent<BtComponent>()->curAct = nullptr;
	}

	void Broadcast() override
	{
		Proto::SyncBtAction pb;
		pb.set_id((int)BtEventId::Flee);
		pb.set_code(0);
		pb.set_npc_sn(_npc->GetSN());
		pb.set_player_sn(0);
		_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncBtAction, pb);
	}

	void Singlecast(Player* player) override
	{
		Proto::SyncBtAction pb;
		pb.set_id((int)BtEventId::Flee);
		pb.set_code(0);
		pb.set_npc_sn(_npc->GetSN());
		pb.set_player_sn(0);
		MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncBtAction, pb, player);
	}

private:
	BtStatus FleeTask()
	{
		if (_npc->GetCurrPos().GetDistance(_npc->GetNextPos()) <= 1)
		{
			if (_npc->hp >= static_cast<int>(_npc->initHp * 0.7))
			{
				_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Patrol);
				return BtStatus::Suspend;
			}
			else
			{
				_currTime = Global::GetInstance()->TimeTick;
				_timeElapsed = _currTime - _lastTime;
				if (_timeElapsed < 1000)
					return BtStatus::Running;
				_lastTime = _currTime;
				_npc->hp = (std::min)(static_cast<int>(_npc->hp + _npc->initHp * 0.05), _npc->initHp);
				Proto::SyncEntityStatus status;
				status.set_sn(_npc->GetSN());
				status.set_hp(_npc->hp);
				_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncEntityStatus, status);
			}
		}
		return BtStatus::Running;
	}
};

#endif // !BT_ACTION_FLEE
