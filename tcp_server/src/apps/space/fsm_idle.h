#ifndef FSM_IDLE
#define FSM_IDLE
#include "fsm_state.h"
#include "fsm_patrol.h"

class Patrol;
class Idle : public FsmState
{
public:
	Idle(Npc* owner, Player* target = nullptr) : FsmState(owner, target) {}

	~Idle() = default;

	void Enter() override;

	void Execute() override;

	void Exit() override;

	void Broadcast() override;

	void Singlecast(Player* pPlayer) override;

	FsmStateType GetStateType() override { return FsmStateType::Idle; }
};

#endif // !FSM_IDLE

