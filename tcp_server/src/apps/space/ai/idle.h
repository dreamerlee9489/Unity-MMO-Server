#ifndef AI_IDLE
#define AI_IDLE
#include "ai_state.h"

class Idle : public AIState
{
public:
	Idle(AIEnemy* owner, Player* target = nullptr) : AIState(owner, target) {}

	~Idle() = default;

	void Enter() override;

	void Execute() override;

	void Exit() override;

	void SyncState() override;
};

#endif // !AI_IDLE

