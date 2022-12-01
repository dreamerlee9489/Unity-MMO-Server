#ifndef AI_PATROL
#define AI_PATROL
#include "ai_component.h"
#include "ai_state.h"
#include "pursuit.h"
#include "../world.h"

class World;
class Patrol : public AIState
{
	int _index = 0;
	std::map<uint64, Player*>* _players = nullptr;
	std::default_random_engine _eng;
	std::uniform_int_distribution<int> _dis = std::uniform_int_distribution<int>(0, 3);

public:
	Patrol(AIEnemy* owner, Player* target = nullptr) : AIState(owner, target)
	{
		_eng = std::default_random_engine(_owner->GetID());
		_players = owner->GetWorld()->GetPlayerManager()->GetAll();
	}
	~Patrol() = default;

	void Enter() override
	{
		_index = _dis(_eng);
		_owner->SetPatrolPoint(_index);
		SyncState();
	}

	void Execute() override
	{
		_currTime = Global::GetInstance()->TimeTick;
		_timeElapsed = _currTime - _lastTime;
		for (auto pair : *_players)
		{
			float dist = _owner->GetCurrPos().GetDistance(pair.second->GetComponent<PlayerComponentLastMap>()->GetCur()->Position);
			if (dist <= 8)
			{
				_owner->GetComponent<AIComponent>()->ChangeState(new Pursuit(_owner, pair.second));
				break;
			}
		}
		if (_timeElapsed >= 10000)
		{
			_lastTime = _currTime;
			_index = _dis(_eng);
			_owner->SetPatrolPoint(_index);
			SyncState();
		}
	}

	void Exit() override
	{
	}

	void SyncState() override
	{
		Proto::FsmChangeState proto;
		proto.set_state((int)AIStateType::Patrol);
		proto.set_code(_index);
		proto.set_enemy_id(_owner->GetID());
		proto.set_player_sn(0);
		_owner->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_FsmChangeState, proto);
	}
};

#endif // !AI_PATROL
