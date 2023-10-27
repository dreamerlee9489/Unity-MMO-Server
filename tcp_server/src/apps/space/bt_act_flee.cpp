#include "bt_act_flee.h"

BtActFlee::BtActFlee(Npc* npc) : BtAction(npc)
{
	_task = std::bind(&BtActFlee::FleeTask, this);
}

BtEventId BtActFlee::GetEventId()
{
	return BtEventId::Flee;
}

void BtActFlee::Enter()
{
	_npc->SetNextPos(_npc->GetInitPos());
	_lastTime = _currTime = Global::GetInstance()->TimeTick;
	_npc->GetComponent<BtComponent>()->curAct = this;
	Broadcast();
}

void BtActFlee::Exit()
{
	_npc->target = nullptr;
	_npc->fleeing = false;
	_npc->GetComponent<BtComponent>()->curAct = nullptr;
}

void BtActFlee::Broadcast()
{
	Proto::SyncBtAction pb;
	pb.set_id((int)BtEventId::Flee);
	pb.set_code(0);
	pb.set_npc_sn(_npc->GetSN());
	pb.set_player_sn(0);
	_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncBtAction, pb);
}

void BtActFlee::Singlecast(Player* player)
{
	Proto::SyncBtAction pb;
	pb.set_id((int)BtEventId::Flee);
	pb.set_code(0);
	pb.set_npc_sn(_npc->GetSN());
	pb.set_player_sn(0);
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncBtAction, pb, player);
}

BtStatus BtActFlee::FleeTask()
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
			Proto::SyncNpcProps status;
			status.set_sn(_npc->GetSN());
			status.set_hp(_npc->hp);
			_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncNpcProps, status);
		}
	}
	return BtStatus::Running;
}
