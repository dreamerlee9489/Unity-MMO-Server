#include "bt_act_attack.h"

BtActAttack::BtActAttack(Npc* npc) : BtAction(npc)
{
	_task = std::bind(&BtActAttack::AttackTask, this);
}

BtEventId BtActAttack::GetEventId()
{
	return BtEventId::Attack;
}

void BtActAttack::Enter()
{
	_npc->GetComponent<BtComponent>()->curAct = this;
	Broadcast();
}

void BtActAttack::Exit()
{
	_npc->GetComponent<BtComponent>()->curAct = nullptr;
}

void BtActAttack::Broadcast()
{
	if (_npc->target)
	{
		Proto::SyncBtAction pb;
		pb.set_id((int)BtEventId::Attack);
		pb.set_code(0);
		pb.set_npc_sn(_npc->GetSN());
		pb.set_player_sn(_npc->target->GetPlayerSN());
		_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncBtAction, pb);
	}
}

void BtActAttack::Singlecast(Player* player)
{
	if (_npc->target)
	{
		Proto::SyncBtAction pb;
		pb.set_id((int)BtEventId::Attack);
		pb.set_code(0);
		pb.set_npc_sn(_npc->GetSN());
		pb.set_player_sn(_npc->target->GetPlayerSN());
		MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncBtAction, pb, player);
	}
}

BtStatus BtActAttack::AttackTask()
{
	if (!_npc->CanAttack(_npc->target))
	{
		_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Pursue);
		return BtStatus::Suspend;
	}
	else if (!_npc->CanSee(_npc->target))
	{
		_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Idle);
		return BtStatus::Suspend;
	}
	return BtStatus::Running;
}
