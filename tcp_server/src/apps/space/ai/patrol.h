#ifndef AI_PATROL
#define AI_PATROL
#include "ai_component.h"
#include "ai_state.h"

class Patrol : public AIState
{
	int _index = 0;
	std::map<uint64, Player*>* _players = nullptr;
	std::default_random_engine _eng;
	std::uniform_int_distribution<int> _dis = std::uniform_int_distribution<int>(0, 3);

public:
	Patrol(AIEnemy* owner, Player* target = nullptr);

	~Patrol() = default;

	void Enter() override;

	void Execute() override;

	void Exit() override;

	void SyncState() override;
};

#endif // !AI_PATROL
