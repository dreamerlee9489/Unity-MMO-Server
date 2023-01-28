#include "fsm_pursuit.h"
#include "fsm_attack.h"
#include "fsm_idle.h"

void Pursuit::Enter()
{
	if (_target != _npc->linker)
	{
		_npc->SetLinkPlayer(_target);
		Proto::ReqLinkPlayer proto;
		proto.set_npc_id(_npc->GetID());
		proto.set_npc_sn(_npc->GetSN());
		proto.set_linker(true);
		MessageSystemHelp::SendPacket(Proto::MsgId::S2C_ReqLinkPlayer, proto, _target);
	}
	Broadcast();
}

void Pursuit::Execute()
{
	if (_npc->CanAttack(_target))
		_npc->GetComponent<FsmComponent>()->ChangeState(new Attack(_npc, _target));
	else if (!_npc->CanSee(_target))
		_npc->GetComponent<FsmComponent>()->ChangeState(new Idle(_npc));
}

void Pursuit::Exit()
{
}

void Pursuit::Broadcast()
{
	Proto::SyncFsmState proto;
	proto.set_state((int)FsmStateType::Pursuit);
	proto.set_code(0);
	proto.set_npc_sn(_npc->GetSN());
	proto.set_player_sn(_target->GetPlayerSN());
	_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncFsmState, proto);
}

void Pursuit::Singlecast(Player* pPlayer)
{
	Proto::SyncFsmState proto;
	proto.set_state((int)FsmStateType::Pursuit);
	proto.set_code(0);
	proto.set_npc_sn(_npc->GetSN());
	proto.set_player_sn(_target->GetPlayerSN());
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncFsmState, proto, pPlayer);
}