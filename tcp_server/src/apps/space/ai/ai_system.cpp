#include "ai_system.h"
#include "ai_enemy.h"
#include "fsm_component.h"

void AISystem::Update(EntitySystem* pEntities)
{
	timeutil::Time curTime = Global::GetInstance()->TimeTick;
	timeutil::Time timeElapsed = curTime - _lastTime;
	if (timeElapsed >= 500)
	{
		_lastTime = curTime;
		if (_pCollections == nullptr)
		{
			_pCollections = pEntities->GetComponentCollections<FsmComponent>();
			if (_pCollections == nullptr)
				return;
		}
		std::map<uint64, IComponent*>* pMap = _pCollections->GetAll();
		for (auto iter = pMap->begin(); iter != pMap->end(); ++iter)
		{
			FsmComponent* pFsmComponent = (FsmComponent*)iter->second;
			AIEnemy* pEnemy = pFsmComponent->GetParent<AIEnemy>();
			pFsmComponent->Update(pEnemy);
			pEnemy->UpdatePos(timeElapsed);
		}
	}
}
