#ifndef AI_SYSTEM
#define AI_SYSTEM
#include "libserver/system.h"
#include "libserver/util_time.h"
#include "libserver/component_collections.h"
#include "libserver/entity_system.h"

class AISystem : public ISystem<AISystem>
{
private:
	timeutil::Time _lastTime;
	ComponentCollections* _pCollections{ nullptr };

public:
	AISystem() { _lastTime = Global::GetInstance()->TimeTick; }

	void Update(EntitySystem* pEntities) override;
};

#endif // !AI_SYSTEM
