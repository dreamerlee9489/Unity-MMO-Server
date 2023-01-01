#ifndef FSM_DEATH
#define FSM_DEATH
#include "fsm_state.h"
#include <vector>

class Death : public FsmState
{
public:
	Death(AIEnemy* owner, Player* target) : FsmState(owner, target) {}

	~Death() = default;

	void Enter() override;

	void Execute() override;

	void Exit() override;

	void Broadcast() override;

	void Singlecast(Player* pPlayer) override;

	FsmStateType GetStateType() override { return FsmStateType::Death; }
};

#endif // !FSM_DEATH

