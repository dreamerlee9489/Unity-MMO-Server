#include "attack.h"
#include "pursuit.h"
#include "idle.h"

Attack::Attack(AIEnemy* owner, Player* target) : FsmState(owner, target)
{
	_type = FsmStateType::Attack;
}

void Attack::Enter()
{
	_lastMap = _target->GetComponent<PlayerComponentLastMap>();
	_owner->SetSpeed(_owner->WalkSpeed);
	BroadcastState();
}

void Attack::Execute()
{
}

void Attack::Exit()
{
}

void Attack::BroadcastState()
{
	Proto::FsmSyncState proto;
	proto.set_state((int)FsmStateType::Attack);
	proto.set_code(0);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(_target->GetPlayerSN());
	_owner->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_FsmSyncState, proto);
}

void Attack::SendState(Player* pPlayer)
{
	Proto::FsmSyncState proto;
	proto.set_state((int)FsmStateType::Attack);
	proto.set_code(0);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(_target->GetPlayerSN());
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_FsmSyncState, proto, pPlayer);
}