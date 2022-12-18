#ifndef FSM_ATTACK
#define FSM_ATTACK
#include "fsm_state.h"

class Attack : public FsmState
{
public:
	Attack(AIEnemy* owner, Player* target = nullptr);

	~Attack() = default;

	void Enter() override;

	void Execute() override;

	void Exit() override;

	void BroadcastState() override;

	void SendState(Player* pPlayer) override;
};
#endif // !FSM_ATTACK
