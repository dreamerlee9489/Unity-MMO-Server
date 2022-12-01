#include "ai_system.h"
#include "ai_component.h"
#include "ai_enemy.h"

void AISystem::Update(EntitySystem* pEntities)
{
	timeutil::Time curTime = Global::GetInstance()->TimeTick;
	timeutil::Time timeElapsed = curTime - _lastTime;
	if (timeElapsed >= 500)
	{
		_lastTime = curTime;
		if (_pCollections == nullptr)
		{
			_pCollections = pEntities->GetComponentCollections<AIComponent>();
			if (_pCollections == nullptr)
				return;
		}
		std::map<uint64, IComponent*>* pMap = _pCollections->GetAll();
		for (auto iter = pMap->begin(); iter != pMap->end(); ++iter)
		{
			AIComponent* pAIComponent = (AIComponent*)iter->second;
			AIEnemy* pEnemy = pAIComponent->GetParent<AIEnemy>();
			pAIComponent->Update(pEnemy);
			pEnemy->UpdatePos(timeElapsed);
		}
	}
}
