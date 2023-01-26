﻿#include "ai_system.h"

AISystem::AISystem()
{
	_lastTime = Global::GetInstance()->TimeTick;
	_currTime = _lastTime;
	_timeElapsed = 0;
}

void AISystem::Update(EntitySystem* pEntities)
{
	_currTime = Global::GetInstance()->TimeTick;
	_timeElapsed = _currTime - _lastTime;
	if (_timeElapsed >= 500)
	{
		_lastTime = _currTime;
		//if (_pCollections == nullptr)
		//{
		//	_pCollections = pEntities->GetComponentCollections<FsmComponent>();
		//	if (_pCollections == nullptr)
		//		return;
		//}
		//std::map<uint64, IComponent*>* pMap = _pCollections->GetAll();
		//for (auto iter = pMap->begin(); iter != pMap->end(); ++iter)
		//{
		//	FsmComponent* pFsmComponent = (FsmComponent*)iter->second;
		//	pFsmComponent->Update();
		//}
		if (_pCollections == nullptr)
		{
			_pCollections = pEntities->GetComponentCollections<BtComponent>();
			if (_pCollections == nullptr)
				return;
		}
		std::map<uint64, IComponent*>* pMap = _pCollections->GetAll();
		for (auto iter = pMap->begin(); iter != pMap->end(); ++iter)
		{
			BtComponent* pBtComponent = (BtComponent*)iter->second;
			pBtComponent->Update();
		}
	}
}
