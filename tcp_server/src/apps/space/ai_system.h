#ifndef AI_SYSTEM
#define AI_SYSTEM
#include "libserver/system.h"
#include "libserver/util_time.h"
#include "libserver/component_collections.h"
#include "libserver/entity_system.h"

class AISystem : public ISystem<AISystem>
{
private:
	timeutil::Time _lastTime, _currTime, _timeElapsed;
	ComponentCollections* _pCollections{ nullptr };

public:
	AISystem();

	void Update(EntitySystem* pEntities) override;
};

#endif // !AI_SYSTEM
