#ifndef AI_ATTACK
#define AI_ATTACK
#include "fsm_state.h"

class Attack : public FsmState
{
	PlayerComponentLastMap* _lastMap = nullptr;

public:
	Attack(AIEnemy* owner, Player* target = nullptr);

	~Attack() = default;

	void Enter() override;

	void Execute() override;

	void Exit() override;

	void BroadcastState() override;

	void SendState(Player* pPlayer) override;
};
#endif // !AI_ATTACK
