﻿#ifndef AI_STATE
#define AI_STATE
#include <map>
#include <cmath>
#include <random>
#include "libplayer/player_component_last_map.h"
#include "libplayer/player.h"
#include "ai_enemy.h"

enum class AIStateType
{
	Idle, Patrol, Pursuit, Attack
};

class AIEnemy;
class Player;
class AIState
{
protected:
	AIEnemy* _owner;
	Player* _target;
	timeutil::Time _lastTime, _currTime, _timeElapsed;

	AIState(AIEnemy* owner, Player* target = nullptr)
		: _owner(owner), _target(target)
	{
		_lastTime = Global::GetInstance()->TimeTick;
		_currTime = _lastTime;
		_timeElapsed = 0;
	}

public:
	virtual ~AIState() = default;

	Player* GetTarget() { return _target; }

	virtual void Enter() = 0;
	virtual void Execute() = 0;
	virtual void Exit() = 0;
	virtual void SyncState() = 0;
};

#endif // !AI_STATE
