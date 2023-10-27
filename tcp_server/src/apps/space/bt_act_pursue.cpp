#include "bt_act_pursue.h"

BtActPursue::BtActPursue(Npc* npc) : BtAction(npc)
{
	_task = std::bind(&BtActPursue::PursueTask, this);
}

BtEventId BtActPursue::GetEventId()
{
	return BtEventId::Pursue;
}

void BtActPursue::Enter()
{
	_npc->GetComponent<BtComponent>()->curAct = this;
	Broadcast();
}

void BtActPursue::Exit()
{
	_npc->GetComponent<BtComponent>()->curAct = nullptr;
}

void BtActPursue::Broadcast()
{
	if (_npc->target)
	{
		Proto::SyncBtAction pb;
		pb.set_id((int)BtEventId::Pursue);
		pb.set_code(0);
		pb.set_npc_sn(_npc->GetSN());
		pb.set_player_sn(_npc->target->GetPlayerSN());
		_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncBtAction, pb);
	}
}

void BtActPursue::Singlecast(Player* player)
{
	if (_npc->target)
	{
		Proto::SyncBtAction pb;
		pb.set_id((int)BtEventId::Pursue);
		pb.set_code(0);
		pb.set_npc_sn(_npc->GetSN());
		pb.set_player_sn(_npc->target->GetPlayerSN());
		MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncBtAction, pb, player);
	}
}

BtStatus BtActPursue::PursueTask()
{
	if (_npc->CanAttack(_npc->target))
	{
		_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Attack);
		return BtStatus::Suspend;
	}
	else if (!_npc->CanSee(_npc->target))
	{
		_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Idle);
		return BtStatus::Suspend;
	}
	return BtStatus::Running;
}
