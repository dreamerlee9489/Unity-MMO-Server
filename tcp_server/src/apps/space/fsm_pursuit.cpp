#include "fsm_pursuit.h"
#include "fsm_attack.h"

Pursuit::Pursuit(AIEnemy* owner, Player* target) : FsmState(owner, target)
{
	_type = FsmStateType::Pursuit;
	_lastMap = _target->GetComponent<PlayerComponentLastMap>();
	_owner->SetNextPos(_lastMap->GetCur()->Position);
}

void Pursuit::Enter()
{
	if (_target != _owner->GetLinkPlayer())
	{
		_owner->SetLinkPlayer(_target);
		Proto::RequestLinkPlayer proto;
		proto.set_enemy_id(_owner->GetID());
		proto.set_islinker(true);
		MessageSystemHelp::SendPacket(Proto::MsgId::S2C_RequestLinkPlayer, proto, _target);
	}
	BroadcastState();
}

void Pursuit::Execute()
{
}

void Pursuit::Exit()
{
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