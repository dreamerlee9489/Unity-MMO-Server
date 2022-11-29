#ifndef AI_PURSUIT
#define AI_PURSUIT
#include "ai_state.h"

class Pursuit : public AIState
{
public:
	Pursuit(AIEnemy* owner, Player* target) : AIState(owner, target) {}
	~Pursuit() = default;

	void Enter()
	{
		LOG_DEBUG("Pursuit enter: id=" << _owner->GetID());
	}

	void Execute()
	{
		LOG_DEBUG("Pursuit exe: id=" << _owner->GetID());
	}

	void Exit()
	{
		LOG_DEBUG("Pursuit enter: id=" << _owner->GetID());
	}
};

#endif // !AI_PURSUIT
