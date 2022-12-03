#include "pursuit.h"
#include "attack.h"

Pursuit::Pursuit(AIEnemy* owner, Player* target) : FsmState(owner, target)
{
	_type = FsmStateType::Pursuit;
}

void Pursuit::Enter()
{
	_owner->SetSpeed(_owner->RunSpeed);
	_lastMap = _target->GetComponent<PlayerComponentLastMap>();
	_owner->SetNextPos(_lastMap->GetCur()->Position);
	BroadcastState();
}

void Pursuit::Execute()
{
}

void Pursuit::Exit()
{
	_owner->SetSpeed(_owner->WalkSpeed);
}

void Pursuit::BroadcastState()
{
	Proto::FsmSyncState proto;
	proto.set_state((int)FsmStateType::Pursuit);
	proto.set_code(0);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(_target->GetPlayerSN());
	_owner->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_FsmSyncState, proto);
}

void Pursuit::SendState(Player* pPlayer)
{
	Proto::FsmSyncState proto;
	proto.set_state((int)FsmStateType::Pursuit);
	proto.set_code(0);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(_target->GetPlayerSN());
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_FsmSyncState, proto, pPlayer);
}