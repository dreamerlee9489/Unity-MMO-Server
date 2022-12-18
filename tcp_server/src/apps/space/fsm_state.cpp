#include "fsm_state.h"
#include "fsm_idle.h"
#include "fsm_patrol.h"
#include "fsm_pursuit.h"
#include "fsm_attack.h"

FsmState::FsmState(AIEnemy* owner, Player* target) : _owner(owner), _target(target)
{
	_lastTime = Global::GetInstance()->TimeTick;
	_currTime = _lastTime;
	_timeElapsed = 0;
}

FsmState* FsmState::GenFsmState(FsmStateType type, AIEnemy* owner, Player* target)
{
	switch (type)
	{
	case FsmStateType::Idle:
		return new Idle(owner);
	case FsmStateType::Patrol:
		return new Patrol(owner);
	case FsmStateType::Pursuit:
		return new Pursuit(owner, target);
	case FsmStateType::Attack:
		return new Attack(owner, target);
	default:
		return nullptr;
	}
}
