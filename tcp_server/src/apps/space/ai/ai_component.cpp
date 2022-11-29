#include "./ai_component.h"
#include "./patrol.h"

void AIComponent::Awake()
{
	_currState = new Patrol((AIEnemy*)GetParent());
	_lastTime = Global::GetInstance()->TimeTick;
}

void AIComponent::BackToPool()
{
	std::cout << "AIComponent BackToPool()\n";
}