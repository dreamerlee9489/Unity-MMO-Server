#ifndef AI_IDLE
#define AI_IDLE
#include "ai_state.h"

class Idle : public AIState
{
public:
	Idle(AIEnemy* owner, Player* target = nullptr) : AIState(owner, target) {}
	~Idle() = default;

	void Enter() override;

	void Execute() override;

	void Exit() override;

	void SyncState() override
	{
		Proto::FsmChangeState proto;
		proto.set_state((int)AIStateType::Idle);
		proto.set_code(0);
		proto.set_enemy_id(_owner->GetID());
		proto.set_player_sn(0);
		_owner->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_FsmChangeState, proto);
	}
};

#endif // !AI_IDLE

