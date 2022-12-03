#include "idle.h"
#include "patrol.h"

void Idle::Enter()
{
	_owner->SetSpeed(_owner->WalkSpeed);
	SyncState();
}

void Idle::Execute()
{
	_currTime = Global::GetInstance()->TimeTick;
	_timeElapsed = _currTime - _lastTime;
	if (_timeElapsed >= 4000)
	{
		_lastTime = _currTime;
		_owner->GetComponent<AIComponent>()->ChangeState(new Patrol((AIEnemy*)_owner));
	}
}

void Idle::Exit()
{
}

void Idle::SyncState()
{
	Proto::FsmChangeState proto;
	proto.set_state((int)AIStateType::Idle);
	proto.set_code(0);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(0);
	_owner->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_FsmChangeState, proto);
}