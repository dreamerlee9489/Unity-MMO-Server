#include "fsm_state.h"
#include "idle.h"
#include "patrol.h"
#include "pursuit.h"
#include "attack.h"

FsmState::FsmState(AIEnemy* owner, Player* target) : _owner(owner), _target(target)
{
	_lastTime = Global::GetInstance()->TimeTick;
	_currTime = _lastTime;
	_timeElapsed = 0;
}

FsmState* FsmState::GenFsmState(FsmStateType type, AIEnemy* owner, Player* target)
{
	FsmState* state = nullptr;
	switch (type)
	{
	case FsmStateType::Idle:
		state = new Idle(owner, nullptr);
		break;
	case FsmStateType::Patrol:
		state = new Patrol(owner, nullptr);
		break;
	case FsmStateType::Pursuit:
		state = new Pursuit(owner, target);
		break;
	case FsmStateType::Attack:
		state = new Attack(owner, target);
		break;
	default:
		break;
	}
	return state;
}
