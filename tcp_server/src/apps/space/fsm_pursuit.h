#ifndef FSM_PURSUIT
#define FSM_PURSUIT
#include "fsm_state.h"

class Pursuit : public FsmState
{
public:
	Pursuit(AIEnemy* owner, Player* target);

	~Pursuit() = default;

	void Enter() override;

	void Execute() override;

	void Exit() override;

	void Broadcast() override;

	void Singlecast(Player* pPlayer) override;
};

#endif // !FSM_PURSUIT
