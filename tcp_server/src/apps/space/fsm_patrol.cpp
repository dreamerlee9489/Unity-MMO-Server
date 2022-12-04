#include "fsm_patrol.h"
#include "fsm_pursuit.h"

Patrol::Patrol(AIEnemy* owner, Player* target) : FsmState(owner, target)
{
	_type = FsmStateType::Patrol;
	_eng = std::default_random_engine(_owner->GetID());
	_players = owner->GetWorld()->GetPlayerManager()->GetAll();
}

void Patrol::Enter()
{
	_index = _dis(_eng);
	_owner->SetPatrolPoint(_index);
	BroadcastState();
}

void Patrol::Execute()
{
	if (++_round >= 12)
	{
		_round = 0;
		_index = _dis(_eng);
		_owner->SetPatrolPoint(_index);
		BroadcastState();
	}
}

void Patrol::Exit()
{
}

void Patrol::BroadcastState()
{
	Proto::FsmSyncState proto;
	proto.set_state((int)FsmStateType::Patrol);
	proto.set_code(_index);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(0);
	_owner->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_FsmSyncState, proto);
}

void Patrol::SendState(Player* pPlayer)
{
	Proto::FsmSyncState proto;
	proto.set_state((int)FsmStateType::Patrol);
	proto.set_code(_index);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(0);
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_FsmSyncState, proto, pPlayer);
}