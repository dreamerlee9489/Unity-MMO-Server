#ifndef AI_PATROL
#define AI_PATROL
#include "ai_component.h"
#include "ai_state.h"
#include "pursuit.h"
#include "../world.h"

class World;
class Patrol : public AIState
{
	std::map<uint64, Player*>* _players = nullptr;

public:
	Patrol(AIEnemy* owner, Player* target = nullptr) : AIState(owner, target)
	{
		_players = owner->GetWorld()->GetPlayerManager()->GetAll();
	}
	~Patrol() = default;

	void Enter()
	{
		LOG_DEBUG("patrol enter: id=" << _owner->GetID());
	}

	void Execute()
	{
		if (!_players->empty())
		{
			for (auto pair : *_players)
			{
				Vector3 dir = pair.second->GetComponent<PlayerComponentLastMap>()->GetCur()->Position - _owner->GetPos();
				if ((dir.X * dir.X + dir.Z * dir.Z) <= 36)
				{
					_owner->GetComponent<AIComponent>()->ChangeState(new Pursuit(_owner, pair.second));
				}
			}
		}
		LOG_DEBUG("patrol exe: id=" << _owner->GetID());
	}

	void Exit()
	{
		LOG_DEBUG("patrol enter: id=" << _owner->GetID());
	}
};

#endif // !AI_PATROL
