#include "ai_state.h"
#include "idle.h"
#include "patrol.h"
#include "pursuit.h"
#include "attack.h"

AIState::AIState(AIEnemy* owner, Player* target) : _owner(owner), _target(target)
{
	_lastTime = Global::GetInstance()->TimeTick;
	_currTime = _lastTime;
	_timeElapsed = 0;
}