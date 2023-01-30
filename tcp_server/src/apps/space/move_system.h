#pragma once
#include "libserver/system.h"
#include "libserver/util_time.h"
#include "libserver/component_collections.h"

class MoveSystem : public ISystem<MoveSystem>
{
public:
	MoveSystem();
	void Update(EntitySystem* pEntities) override;

private:
	timeutil::Time _lastTime = 0, _currTime = 0, _timeElapsed = 0;
	ComponentCollections* _pCollections{ nullptr };
};

