#ifndef AI_SYSTEM
#define AI_SYSTEM
#include "libserver/system.h"
#include "libserver/util_time.h"
#include "libserver/component_collections.h"
#include "libserver/entity_system.h"
#include "ai_component.h"
#include "ai_enemy.h"

class AISystem : public ISystem<AISystem>
{
private:
	timeutil::Time _lastTime;
	ComponentCollections* _pCollections{ nullptr };

public:
	AISystem() { _lastTime = Global::GetInstance()->TimeTick; }

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
				AIEnemy* pEnemy = pAIComponent->GetParent<AIEnemy>();
				pAIComponent->Update(pEnemy);
			}
		}
	}
};

#endif // !AI_SYSTEM
