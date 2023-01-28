#include "fsm_attack.h"
#include "fsm_pursuit.h"
#include "fsm_idle.h"

void Attack::Enter()
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

void Attack::Execute()
{
	if (!_npc->CanAttack(_target))
		_npc->GetComponent<FsmComponent>()->ChangeState(new Pursuit(_npc, _target));
	else if (!_npc->CanSee(_target))
		_npc->GetComponent<FsmComponent>()->ChangeState(new Idle(_npc));
}

void Attack::Exit()
{
}

void Attack::Broadcast()
{
	Proto::SyncFsmState proto;
	proto.set_state((int)FsmStateType::Attack);
	proto.set_code(0);
	proto.set_npc_sn(_npc->GetSN());
	proto.set_player_sn(_target->GetPlayerSN());
	_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncFsmState, proto);
}

void Attack::Singlecast(Player* pPlayer)
{
	Proto::SyncFsmState proto;
	proto.set_state((int)FsmStateType::Attack);
	proto.set_code(0);
	proto.set_npc_sn(_npc->GetSN());
	proto.set_player_sn(_target->GetPlayerSN());
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncFsmState, proto, pPlayer);
}
