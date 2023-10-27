#include "bt_act_birth.h"

BtActBirth::BtActBirth(Npc* npc) : BtAction(npc)
{
	_task = std::bind(&BtActBirth::BirthTask, this);
}

BtEventId BtActBirth::GetEventId()
{
	return BtEventId::Birth;
}

void BtActBirth::Enter()
{
	_npc->target = nullptr;
	_npc->linker = nullptr;
	_npc->fleeing = false;
	_npc->SetCurrPos(_npc->GetInitPos());
	_currTime = _lastTime = Global::GetInstance()->TimeTick;
}

BtStatus BtActBirth::BirthTask()
{
	_currTime = Global::GetInstance()->TimeTick;
	if (_npc->GetWorld()->playerMgr->GetAll()->empty())
	{
		_lastTime = _currTime;
		return BtStatus::Running;
	}
	_timeElapsed = _currTime - _lastTime;
	if (_timeElapsed < 2000)
		return BtStatus::Running;
	_npc->hp = _npc->initHp;
	return BtStatus::Success;
}

void BtActBirth::Broadcast()
{
	Proto::SyncBtAction pb;
	pb.set_id((int)BtEventId::Birth);
	pb.set_code(0);
	pb.set_npc_sn(_npc->GetSN());
	pb.set_player_sn(0);
	_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncBtAction, pb);
}

void BtActBirth::Singlecast(Player* player)
{
	Proto::SyncBtAction pb;
	pb.set_id((int)BtEventId::Birth);
	pb.set_code(0);
	pb.set_npc_sn(_npc->GetSN());
	pb.set_player_sn(0);
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncBtAction, pb, player);
}
