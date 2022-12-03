#ifndef AI_ATTACK
#define AI_ATTACK
#include "ai_state.h"

class Attack : public AIState
{
	PlayerComponentLastMap* _lastMap = nullptr;

public:
	Attack(AIEnemy* owner, Player* target = nullptr) : AIState(owner, target) {}

	~Attack() = default;

	void Enter() override;

	void Execute() override;

	void Exit() override;

	void SyncState() override;
};
#endif // !AI_ATTACK
