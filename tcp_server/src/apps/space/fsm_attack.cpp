#include "fsm_attack.h"
#include "fsm_pursuit.h"
#include "fsm_idle.h"

Attack::Attack(AIEnemy* owner, Player* target) : FsmState(owner, target)
{
	_type = FsmStateType::Attack;
	_lastMap = _target->GetComponent<PlayerComponentLastMap>();
}

void Attack::Enter()
{
	if (_target != _owner->GetLinkPlayer())
	{
		_owner->SetLinkPlayer(_target);
		Net::RequestLinkPlayer proto;
		proto.set_enemy_id(_owner->GetID());
		proto.set_islinker(true);
		MessageSystemHelp::SendPacket(Net::MsgId::S2C_RequestLinkPlayer, proto, _target);
	}
	BroadcastState();
}

void Attack::Execute()
{
	if (!_owner->CanAttack(_target))
		_owner->GetComponent<FsmComponent>()->ChangeState(new Pursuit(_owner, _target));
	else if (!_owner->CanSee(_target))
		_owner->GetComponent<FsmComponent>()->ChangeState(new Idle(_owner));
}

void Attack::Exit()
{
}

void Attack::BroadcastState()
{
	Net::FsmSyncState proto;
	proto.set_state((int)FsmStateType::Attack);
	proto.set_code(0);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(_target->GetPlayerSN());
	_owner->GetWorld()->BroadcastPacket(Net::MsgId::S2C_FsmSyncState, proto);
}

void Attack::SendState(Player* pPlayer)
{
	Net::FsmSyncState proto;
	proto.set_state((int)FsmStateType::Attack);
	proto.set_code(0);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(_target->GetPlayerSN());
	MessageSystemHelp::SendPacket(Net::MsgId::S2C_FsmSyncState, proto, pPlayer);
}