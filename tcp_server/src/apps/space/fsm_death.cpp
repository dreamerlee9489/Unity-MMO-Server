#include "fsm_death.h"

void Death::Enter()
{
	Broadcast();
}

void Death::Execute()
{
}

void Death::Exit()
{
}

void Death::Broadcast()
{
	Proto::SyncFsmState proto;
	proto.set_state((int)FsmStateType::Death);
	proto.set_code(0);
	proto.set_npc_sn(_owner->GetSN());
	proto.set_player_sn(0);
	_owner->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncFsmState, proto);
}

void Death::Singlecast(Player* pPlayer)
{
	Proto::SyncFsmState proto;
	proto.set_state((int)FsmStateType::Death);
	proto.set_code(0);
	proto.set_npc_sn(_owner->GetSN());
	proto.set_player_sn(0);
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncFsmState, proto, pPlayer);
}
