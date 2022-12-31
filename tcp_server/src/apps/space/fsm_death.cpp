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
	Proto::FsmSyncState proto;
	proto.set_state((int)FsmStateType::Death);
	proto.set_code(0);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(0);
	_owner->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_FsmSyncState, proto);
}

void Death::Singlecast(Player* pPlayer)
{
	Proto::FsmSyncState proto;
	proto.set_state((int)FsmStateType::Death);
	proto.set_code(0);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(0);
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_FsmSyncState, proto, pPlayer);
}
