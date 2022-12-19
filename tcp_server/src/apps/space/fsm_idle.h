#ifndef FSM_IDLE
#define FSM_IDLE
#include "fsm_state.h"
#include "fsm_patrol.h"

class Patrol;
class Idle : public FsmState
{
public:
	Idle(AIEnemy* owner, Player* target = nullptr);

	~Idle() = default;

	void Enter() override;

	void Execute() override;

	void Exit() override;

	void Broadcast() override;

	void Singlecast(Player* pPlayer) override;
};

#endif // !FSM_IDLE

