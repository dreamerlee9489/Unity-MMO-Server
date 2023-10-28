#include "ai_system.h"
#include "bt_component.h"

AISystem::AISystem()
{
	_currTime = _lastTime = Global::GetInstance()->TimeTick;
}

void AISystem::Update(EntitySystem* pEntities)
{
	_currTime = Global::GetInstance()->TimeTick;
	_timeElapsed = _currTime - _lastTime;
	if (_timeElapsed < 100)
		return;
	_lastTime = _currTime;
	if (_pCollections == nullptr)
	{
		_pCollections = pEntities->GetComponentCollections<BtComponent>();
		if (_pCollections == nullptr)
			return;
	}
	auto& map = *_pCollections->GetAll();
	for (auto iter = map.begin(); iter != map.end(); ++iter)
		static_cast<BtComponent*>(iter->second)->Update();
}
