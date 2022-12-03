#ifndef AI_PURSUIT
#define AI_PURSUIT
#include "ai_state.h"

class Pursuit : public AIState
{
	PlayerComponentLastMap* _lastMap = nullptr;

public:
	Pursuit(AIEnemy* owner, Player* target) : AIState(owner, target) {}

	~Pursuit() = default;

	void Enter() override;

	void Execute() override;

	void Exit() override;

	void SyncState() override;
};

#endif // !AI_PURSUIT
