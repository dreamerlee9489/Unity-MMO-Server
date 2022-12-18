﻿#include "fsm_patrol.h"
#include "fsm_pursuit.h"

Patrol::Patrol(AIEnemy* owner, Player* target) : FsmState(owner, target)
{
	_currTime = Global::GetInstance()->TimeTick;
	_type = FsmStateType::Patrol;
	_eng = std::default_random_engine(_currTime % UINT32_MAX + _owner->GetID());
}

void Patrol::Enter()
{
	_index = _dis(_eng);
	_owner->SetPatrolPoint(_index);
	BroadcastState();
}

void Patrol::Execute()
{
	float dist = _owner->GetCurrPos().GetDistance(_owner->GetNextPos());
	if (dist <= 1)
		_owner->GetComponent<FsmComponent>()->ChangeState(new Idle(_owner));
}

void Patrol::Exit()
{
}

void Patrol::BroadcastState()
{
	Net::FsmSyncState proto;
	proto.set_state((int)FsmStateType::Patrol);
	proto.set_code(_index);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(0);
	_owner->GetWorld()->BroadcastPacket(Net::MsgId::S2C_FsmSyncState, proto);
}

void Patrol::SendState(Player* pPlayer)
{
	Net::FsmSyncState proto;
	proto.set_state((int)FsmStateType::Patrol);
	proto.set_code(_index);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(0);
	MessageSystemHelp::SendPacket(Net::MsgId::S2C_FsmSyncState, proto, pPlayer);
}