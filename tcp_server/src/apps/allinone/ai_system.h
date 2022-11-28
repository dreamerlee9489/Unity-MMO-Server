#pragma once
#include "libserver/system.h"
#include "libserver/util_time.h"
#include "libserver/component_collections.h"
#include "libserver/entity_system.h"
#include "../../libs/libplayer/enemy.h"
#include "ai_component.h"
#include <iostream>

class AISystem : public ISystem<AISystem>
{
public:
	AISystem()
	{
		_lastTime = Global::GetInstance()->TimeTick;
	}

	void Update(EntitySystem* pEntities) override
	{
		timeutil::Time curTime = Global::GetInstance()->TimeTick;
		timeutil::Time timeElapsed = curTime - _lastTime;
		if (timeElapsed >= 500)
		{
			if (_pCollections == nullptr)
			{
				_pCollections = pEntities->GetComponentCollections<AIComponent>();
				if (_pCollections == nullptr)
					return;
			}
			_lastTime = curTime;
			std::map<uint64, IComponent*>* pMap = _pCollections->GetAll();
			for (auto iter = pMap->begin(); iter != pMap->end(); ++iter)
			{
				AIComponent* pAIComponent = (AIComponent*)iter->second;
				Enemy* pEnemy = pAIComponent->GetParent<Enemy>();
				pAIComponent->Update(pEnemy);
			}
		}
	}

private:
	timeutil::Time _lastTime;
	ComponentCollections* _pCollections{ nullptr };
};