#include "./ai_component.h"
#include "./patrol.h"
#include "./idle.h"

AIComponent::~AIComponent()
{
	delete _currState;
	_currState = nullptr;
}

void AIComponent::Awake()
{
	_currState = new Idle((AIEnemy*)GetParent());
	_lastTime = Global::GetInstance()->TimeTick;
	//AddTimer(0, 1, false, 0, BindFunP0(this, &AIComponent::SyncAIStateTimer));
}

void AIComponent::BackToPool()
{
}

void AIComponent::Update(AIEnemy* pEnemy)
{
	if (_currState != nullptr)
		_currState->Execute();
}

void AIComponent::ResetState()
{
	ChangeState(new Idle((AIEnemy*)GetParent(), nullptr));
}

void AIComponent::ChangeState(AIState* newState)
{
	if (_prevState)
		delete _prevState;
	_prevState = _currState;
	_currState->Exit();
	_currState = newState;
	_currState->Enter();
}

void AIComponent::SyncAIStateTimer()
{
	if (_currState)
		_currState->SyncState();
}
