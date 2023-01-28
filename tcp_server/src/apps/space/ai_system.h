#ifndef AI_SYSTEM
#define AI_SYSTEM
#include "libserver/system.h"
#include "libserver/util_time.h"
#include "libserver/component_collections.h"
#include "libserver/entity_system.h"
#include "libserver/entity.h"
//#include "fsm_component.h"
#include "bt_component.h"

class AISystem : public ISystem<AISystem>
{
private:
	timeutil::Time _lastTime = 0, _currTime = 0, _timeElapsed = 0;
	ComponentCollections* _pCollections{ nullptr };

public:
	AISystem();

	void Update(EntitySystem* pEntities) override;
};

#endif // !AI_SYSTEM
