#include "bt_act_death.h"

BtActDeath::BtActDeath(Npc* npc) : BtAction(npc)
{
	_task = std::bind(&BtActDeath::DeathTask, this);
}

BtEventId BtActDeath::GetEventId()
{
	return BtEventId::Birth;
}

void BtActDeath::Enter()
{
	_npc->target = nullptr;
	_npc->linker = nullptr;
	_currTime = _lastTime = Global::GetInstance()->TimeTick;
	Broadcast();
}

BtStatus BtActDeath::DeathTask()
{
	_currTime = Global::GetInstance()->TimeTick;
	_timeElapsed = _currTime - _lastTime;
	if (_timeElapsed < 8000 || !_npc->rebirth)
		return BtStatus::Running;
	return BtStatus::Success;
}

void BtActDeath::Broadcast()
{
	Proto::SyncBtAction pb;
	pb.set_id((int)BtEventId::Death);
	pb.set_code(0);
	pb.set_npc_sn(_npc->GetSN());
	pb.set_player_sn(0);
	_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncBtAction, pb);
}

void BtActDeath::Singlecast(Player* player)
{
	Proto::SyncBtAction pb;
	pb.set_id((int)BtEventId::Death);
	pb.set_code(0);
	pb.set_npc_sn(_npc->GetSN());
	pb.set_player_sn(0);
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncBtAction, pb, player);
}
