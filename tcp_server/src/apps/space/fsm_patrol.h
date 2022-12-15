﻿#ifndef FSM_PATROL
#define FSM_PATROL
#include "fsm_component.h"
#include "fsm_state.h"
#include "fsm_idle.h"

class Idle;
class Patrol : public FsmState
{
	int _index = 0;
	std::default_random_engine _eng;
	std::uniform_int_distribution<int> _dis = std::uniform_int_distribution<int>(0, 3);

public:
	Patrol(AIEnemy* owner, Player* target = nullptr);

	~Patrol() = default;

	void Enter() override;

	void Execute() override;

	void Exit() override;

	void BroadcastState() override;

	void SendState(Player* pPlayer) override;
};

#endif // !FSM_PATROL
