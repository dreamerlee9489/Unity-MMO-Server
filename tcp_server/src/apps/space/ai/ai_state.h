#ifndef AI_STATE
#define AI_STATE
#include "libplayer/player.h"
#include "ai_enemy.h"
#include <map>
#include <cmath>

class AIState
{
protected:
	AIEnemy* _owner;
	Player* _target;

	AIState(AIEnemy* owner, Player* target = nullptr)
		: _owner(owner), _target(target) {}

public:
	virtual ~AIState() = default;
	virtual void Enter() = 0;
	virtual void Execute() = 0;
	virtual void Exit() = 0;
};

#endif // !AI_STATE
