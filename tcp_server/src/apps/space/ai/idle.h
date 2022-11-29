#ifndef AI_IDLE
#define AI_IDLE
#include "ai_state.h"

class Idle : public AIState
{
public:
	Idle(AIEnemy* owner, Player* target = nullptr) : AIState(owner, target) {}
	~Idle() = default;

	void Enter()
	{
		LOG_DEBUG("idle enter: id=" << _owner->GetID());
	}

	void Execute()
	{
		LOG_DEBUG("idle exe: id=" << _owner->GetID());
	}

	void Exit()
	{
		LOG_DEBUG("idle exit: id=" << _owner->GetID());
	}
};

#endif // !AI_IDLE

