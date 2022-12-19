#include "fsm_pursuit.h"
#include "fsm_attack.h"
#include "fsm_idle.h"

Pursuit::Pursuit(AIEnemy* owner, Player* target) : FsmState(owner, target)
{
	_type = FsmStateType::Pursuit;
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
	Broadcast();
}

void Pursuit::Execute()
{
	if (_owner->CanAttack(_target))
		_owner->GetComponent<FsmComponent>()->ChangeState(new Attack(_owner, _target));
	else if (!_owner->CanSee(_target))
		_owner->GetComponent<FsmComponent>()->ChangeState(new Idle(_owner));
}

void Pursuit::Exit()
{
}

void Pursuit::Broadcast()
{
	Proto::FsmSyncState proto;
	proto.set_state((int)FsmStateType::Pursuit);
	proto.set_code(0);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(_target->GetPlayerSN());
	_owner->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_FsmSyncState, proto);
}

void Pursuit::Singlecast(Player* pPlayer)
{
	Proto::FsmSyncState proto;
	proto.set_state((int)FsmStateType::Pursuit);
	proto.set_code(0);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(_target->GetPlayerSN());
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_FsmSyncState, proto, pPlayer);
}