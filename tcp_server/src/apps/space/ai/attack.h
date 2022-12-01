#ifndef AI_ATTACK
#define AI_ATTACK
#include "ai_state.h"

class Attack : public AIState
{
	PlayerComponentLastMap* _lastMap = nullptr;

public:
	Attack(AIEnemy* owner, Player* target = nullptr) : AIState(owner, target) {}
	~Attack() = default;

	void Enter() override;

	void Execute() override;

	void Exit() override;

	void SyncState() override
	{
		Proto::FsmChangeState proto;
		proto.set_state((int)AIStateType::Attack);
		proto.set_code(0);
		proto.set_enemy_id(_owner->GetID());
		proto.set_player_sn(_target->GetPlayerSN());
		_owner->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_FsmChangeState, proto);
	}
};
#endif // !AI_ATTACK
