#ifndef AI_IDLE
#define AI_IDLE
#include "fsm_state.h"

class Idle : public FsmState
{
public:
	Idle(AIEnemy* owner, Player* target = nullptr);

	~Idle() = default;

	void Enter() override;

	void Execute() override;

	void Exit() override;

	void BroadcastState() override;

	void SendState(Player* pPlayer) override;
};

#endif // !AI_IDLE

