#include "fsm_idle.h"
#include "fsm_patrol.h"

Idle::Idle(AIEnemy* owner, Player* target) : FsmState(owner, target)
{
	_type = FsmStateType::Idle;
}

void Idle::Enter()
{
	BroadcastState();
}

void Idle::Execute()
{
	_currTime = Global::GetInstance()->TimeTick;
	_timeElapsed = _currTime - _lastTime;
	if (_timeElapsed >= 2000)
	{
		_lastTime = _currTime;
		_owner->GetComponent<FsmComponent>()->ChangeState(new Patrol(_owner));
	}
}

void Idle::Exit()
{
}

void Idle::BroadcastState()
{
	Proto::FsmSyncState proto;
	proto.set_state((int)FsmStateType::Idle);
	proto.set_code(0);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(0);
	_owner->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_FsmSyncState, proto);
}

void Idle::SendState(Player* pPlayer)
{
	Proto::FsmSyncState proto;
	proto.set_state((int)FsmStateType::Idle);
	proto.set_code(0);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(0);
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_FsmSyncState, proto, pPlayer);
}