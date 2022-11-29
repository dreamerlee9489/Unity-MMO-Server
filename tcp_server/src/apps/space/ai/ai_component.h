#ifndef AI_COMPONENT
#define AI_COMPONENT
#include "libserver/component.h"
#include "libserver/system.h"
#include "libserver/vector3.h"
#include "ai_enemy.h"
#include "ai_state.h"

class AIComponent :public Component<AIComponent>, public IAwakeFromPoolSystem<>
{
	timeutil::Time _lastTime;
	AIState* _prevState = nullptr;
	AIState* _currState = nullptr;

public:
	~AIComponent()
	{
		delete _currState;
		_currState = nullptr;
		std::cout << "~AIComponent()\n";
	}

	void Awake() override;

	void BackToPool() override;

	void Update(AIEnemy* pEnemy)
	{
		timeutil::Time curTime = Global::GetInstance()->TimeTick;
		timeutil::Time timeElapsed = curTime - _lastTime;
		if (_currState != nullptr)
			_currState->Execute();

		if (timeElapsed >= 10000)
		{
			_lastTime = curTime;
			pEnemy->SetRandPos();
		}
	}

	void ChangeState(AIState* newState)
	{
		if (_prevState)
			delete _prevState;
		_prevState = _currState;
		_currState->Exit();
		_currState = newState;
		_currState->Enter();
	}
};

#endif // !AI_COMPONENT


