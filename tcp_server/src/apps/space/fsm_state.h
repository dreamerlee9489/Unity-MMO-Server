#ifndef FSM_STATE
#define FSM_STATE
#include <map>
#include <cmath>
#include <random>
#include <stdexcept>
#include "libplayer/player_component_last_map.h"
#include "libplayer/player.h"
#include "ai_enemy.h"

enum class FsmStateType { Idle, Patrol, Pursuit, Attack, Death };

class AIEnemy;
class Player;
class FsmState
{
protected:
	AIEnemy* _owner = nullptr;
	Player* _target = nullptr;
	timeutil::Time _lastTime, _currTime, _timeElapsed;

	FsmState(AIEnemy* owner, Player* target = nullptr);

public:
	virtual ~FsmState() = default;
	virtual void Enter() = 0;
	virtual void Execute() = 0;
	virtual void Exit() = 0;
	virtual void Broadcast() = 0;
	virtual void Singlecast(Player* player) = 0;
	virtual FsmStateType GetStateType() = 0;

	Player* GetTarget() { return _target; }
	static FsmState* GenFsmState(FsmStateType type, AIEnemy* owner, Player* target);
};

#endif // !FSM_STATE
