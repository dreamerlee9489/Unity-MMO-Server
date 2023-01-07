#include "fsm_pursuit.h"
#include "fsm_attack.h"
#include "fsm_idle.h"

void Pursuit::Enter()
{
	if (_target != _owner->GetLinkPlayer())
	{
		_owner->SetLinkPlayer(_target);
		Proto::ReqLinkPlayer proto;
		proto.set_npc_id(_owner->GetID());
		proto.set_npc_sn(_owner->GetSN());
		proto.set_linker(true);
		MessageSystemHelp::SendPacket(Proto::MsgId::S2C_ReqLinkPlayer, proto, _target);
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
	Proto::SyncFsmState proto;
	proto.set_state((int)FsmStateType::Pursuit);
	proto.set_code(0);
	proto.set_npc_sn(_owner->GetSN());
	proto.set_player_sn(_target->GetPlayerSN());
	_owner->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncFsmState, proto);
}

void Pursuit::Singlecast(Player* pPlayer)
{
	Proto::SyncFsmState proto;
	proto.set_state((int)FsmStateType::Pursuit);
	proto.set_code(0);
	proto.set_npc_sn(_owner->GetSN());
	proto.set_player_sn(_target->GetPlayerSN());
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncFsmState, proto, pPlayer);
}