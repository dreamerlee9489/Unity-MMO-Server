#ifndef FSM_STATE
#define FSM_STATE
#include <map>
#include <cmath>
#include <random>
#include <stdexcept>
#include "libplayer/player_component_last_map.h"
#include "libplayer/player.h"
#include "npc.h"

enum struct FsmStateType { Idle, Patrol, Pursuit, Attack, Death };

class Npc;
class Player;
class FsmState
{
protected:
	Npc* _npc = nullptr;
	Player* _target = nullptr;
	timeutil::Time _lastTime, _currTime, _timeElapsed;

	FsmState(Npc* owner, Player* target = nullptr);

public:
	virtual ~FsmState() = default;
	virtual void Enter() = 0;
	virtual void Execute() = 0;
	virtual void Exit() = 0;
	virtual void Broadcast() = 0;
	virtual void Singlecast(Player* player) = 0;
	virtual FsmStateType GetStateType() = 0;

	Player* GetTarget() { return _target; }
	static FsmState* GenFsmState(FsmStateType type, Npc* owner, Player* target);
};

#endif // !FSM_STATE
