#ifndef FSM_PURSUIT
#define FSM_PURSUIT
#include "fsm_state.h"

class Pursuit : public FsmState
{
	PlayerComponentLastMap* _lastMap = nullptr;

public:
	Pursuit(AIEnemy* owner, Player* target);

	~Pursuit() = default;

	void Enter() override;

	void Execute() override;

	void Exit() override;

	void BroadcastState() override;

	void SendState(Player* pPlayer) override;
};

#endif // !FSM_PURSUIT
