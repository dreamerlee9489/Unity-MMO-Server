#include "bt_act_idle.h"

BtActIdle::BtActIdle(Npc* npc) : BtAction(npc)
{
	_task = std::bind(&BtActIdle::IdleTask, this);
}

BtEventId BtActIdle::GetEventId()
{
	return BtEventId::Idle;
}

void BtActIdle::Enter()
{
	_npc->target = nullptr;
	_lastTime = _currTime = Global::GetInstance()->TimeTick;
	_npc->GetComponent<BtComponent>()->curAct = this;
	Broadcast();
}

void BtActIdle::Exit()
{
	_npc->GetComponent<BtComponent>()->curAct = nullptr;
}

void BtActIdle::Broadcast()
{
	Proto::SyncBtAction pb;
	pb.set_id((int)BtEventId::Idle);
	pb.set_code(0);
	pb.set_npc_sn(_npc->GetSN());
	pb.set_player_sn(0);
	_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncBtAction, pb);
}

void BtActIdle::Singlecast(Player* player)
{
	Proto::SyncBtAction pb;
	pb.set_id((int)BtEventId::Idle);
	pb.set_code(0);
	pb.set_npc_sn(_npc->GetSN());
	pb.set_player_sn(0);
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncBtAction, pb, player);
}

BtStatus BtActIdle::IdleTask()
{
	_currTime = Global::GetInstance()->TimeTick;
	_timeElapsed = _currTime - _lastTime;
	if (_timeElapsed < 2000)
		return BtStatus::Running;
	_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Patrol);
	return BtStatus::Suspend;
}
