#ifndef FSM_PATROL
#define FSM_PATROL
#include "fsm_component.h"
#include "fsm_state.h"
#include "fsm_idle.h"

class Idle;
class Patrol : public FsmState
{
	int _index = 0;
	static std::default_random_engine _eng;
	static std::uniform_int_distribution<int> _dis;

public:
	Patrol(Npc* owner, Player* target = nullptr) : FsmState(owner, target) {}

	~Patrol() = default;

	void Enter() override;

	void Execute() override;

	void Exit() override;

	void Broadcast() override;

	void Singlecast(Player* pPlayer) override;

	FsmStateType GetStateType() override { return FsmStateType::Patrol; }
};

#endif // !FSM_PATROL
