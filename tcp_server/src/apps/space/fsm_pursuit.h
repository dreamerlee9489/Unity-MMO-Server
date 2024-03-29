﻿#ifndef FSM_PURSUIT
#define FSM_PURSUIT
#include "fsm_state.h"

class Pursuit : public FsmState
{
public:
	Pursuit(Npc* owner, Player* target) : FsmState(owner, target) {}

	~Pursuit() = default;

	void Enter() override;

	void Execute() override;

	void Exit() override;

	void Broadcast() override;

	void Singlecast(Player* pPlayer) override;

	FsmStateType GetStateType() override { return FsmStateType::Pursuit; }
};

#endif // !FSM_PURSUIT
