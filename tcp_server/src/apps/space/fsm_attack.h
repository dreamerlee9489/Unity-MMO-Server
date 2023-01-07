#ifndef FSM_ATTACK
#define FSM_ATTACK
#include "fsm_state.h"

class Attack : public FsmState
{
public:
	Attack(Npc* owner, Player* target = nullptr) : FsmState(owner, target) {}

	~Attack() = default;

	void Enter() override;

	void Execute() override;

	void Exit() override;

	void Broadcast() override;

	void Singlecast(Player* pPlayer) override;

	FsmStateType GetStateType() override { return FsmStateType::Attack; }
};
#endif // !FSM_ATTACK
