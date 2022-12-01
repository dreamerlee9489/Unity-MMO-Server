﻿#ifndef AI_PURSUIT
#define AI_PURSUIT
#include "ai_state.h"

class Pursuit : public AIState
{
	PlayerComponentLastMap* _lastMap = nullptr;

public:
	Pursuit(AIEnemy* owner, Player* target) : AIState(owner, target) {}
	~Pursuit() = default;

	void Enter() override;

	void Execute() override;

	void Exit() override;

	void SyncState() override
	{
		Proto::FsmChangeState proto;
		proto.set_state((int)AIStateType::Pursuit);
		proto.set_code(0);
		proto.set_enemy_id(_owner->GetID());
		proto.set_player_sn(_target->GetPlayerSN());
		_owner->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_FsmChangeState, proto);
	}
};

#endif // !AI_PURSUIT
