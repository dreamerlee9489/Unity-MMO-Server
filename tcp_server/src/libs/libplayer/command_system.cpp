#include "command_system.h"

CmdSystem::CmdSystem()
{
	_currTime = _lastTime = Global::GetInstance()->TimeTick;
}

void CmdSystem::Update(EntitySystem* pEntities)
{
	_currTime = Global::GetInstance()->TimeTick;
	_timeElapsed = _currTime - _lastTime;
	if (_timeElapsed < 100)
		return;
	_lastTime = _currTime;
	if (_pCollections == nullptr)
	{
		_pCollections = pEntities->GetComponentCollections<CmdComponent>();
		if (_pCollections == nullptr)
			return;
	}
	auto& map = *_pCollections->GetAll();
	for (auto iter = map.begin(); iter != map.end(); ++iter)
		static_cast<CmdComponent*>(iter->second)->Update();
}
