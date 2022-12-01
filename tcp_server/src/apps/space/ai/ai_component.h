#ifndef AI_COMPONENT
#define AI_COMPONENT
#include "libserver/component.h"
#include "libserver/system.h"
#include "libserver/vector3.h"
#include "ai_enemy.h"
#include "ai_state.h"

class AIEnemy;
class AIState;
class AIComponent :public Component<AIComponent>, public IAwakeFromPoolSystem<>
{
	timeutil::Time _lastTime;
	AIState* _prevState = nullptr;
	AIState* _currState = nullptr;

public:
	~AIComponent();

	void Awake() override;

	void BackToPool() override;

	void Update(AIEnemy* pEnemy);

	void ResetState();

	AIState* GetCurrState() { return _currState; }

	void ChangeState(AIState* newState);

	void SyncAIStateTimer();
};

#endif // !AI_COMPONENT


