#include "bt_act_patrol.h"

std::default_random_engine BtActPatrol::_eng = std::default_random_engine();
std::uniform_int_distribution<int> BtActPatrol::_dis = std::uniform_int_distribution<int>(0, 3);

BtActPatrol::BtActPatrol(Npc* npc) : BtAction(npc)
{
	_task = std::bind(&BtActPatrol::PatrolTask, this);
	_index = _dis(_eng);
	_npc->SetPatrolPoint(_index);
}

BtEventId BtActPatrol::GetEventId()
{
	return BtEventId::Patrol;
}

void BtActPatrol::Enter()
{
	_index = _dis(_eng);
	_npc->SetPatrolPoint(_index);
	_npc->target = nullptr;
	_npc->GetComponent<BtComponent>()->curAct = this;
	Broadcast();
}

void BtActPatrol::Exit()
{
	_npc->GetComponent<BtComponent>()->curAct = nullptr;
}

void BtActPatrol::Broadcast()
{
	Proto::SyncBtAction pb;
	pb.set_id((int)BtEventId::Patrol);
	pb.set_code(_index);
	pb.set_npc_sn(_npc->GetSN());
	pb.set_player_sn(0);
	_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncBtAction, pb);
}

void BtActPatrol::Singlecast(Player* player)
{
	Proto::SyncBtAction pb;
	pb.set_id((int)BtEventId::Patrol);
	pb.set_code(_index);
	pb.set_npc_sn(_npc->GetSN());
	pb.set_player_sn(0);
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncBtAction, pb, player);
}

BtStatus BtActPatrol::PatrolTask()
{
	if (_npc->GetCurrPos().GetDistance(_npc->GetNextPos()) <= 1)
	{
		_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Idle);
		return BtStatus::Suspend;
	}
	return BtStatus::Running;
}
