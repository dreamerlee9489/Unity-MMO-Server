#include "idle.h"
#include "patrol.h"

void Idle::Enter()
{
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