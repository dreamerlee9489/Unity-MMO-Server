#pragma once
#include "libserver/component.h"
#include "libserver/system.h"
#include "libserver/vector3.h"
#include "libplayer/enemy.h"
#include <iostream>
#include <random>

class AIComponent :public Component<AIComponent>, public IAwakeFromPoolSystem<>
{
	timeutil::Time _lastTime;
	Enemy* _pEnemy;
	Vector3 _initPos{ 0, 0, 0 };
	std::default_random_engine eng;
	std::uniform_real_distribution<float> dis = std::uniform_real_distribution<float>(-6.0, 6.0);

public:
	void Awake() override
	{
		_lastTime = Global::GetInstance()->TimeTick;
		_initPos = GetParent<Enemy>()->GetPos();
	}

	void BackToPool() override
	{
		std::cout << "AIComponent BackToPool()\n";
	}

	// 0.5s调用一次
	void Update(Enemy* pEnemy)
	{
		//std::cout << "AIComponent Update(): id=" << pEnemy->GetID() << " " << pEnemy->GetHP() << "\n";
		/*if (_pEnemy == nullptr)
		{
			_pEnemy = pEnemy;
			_initPos.X = _pEnemy->GetPos().X;
			_initPos.Y = _pEnemy->GetPos().Y;
			_initPos.Z = _pEnemy->GetPos().Z;
		}*/
		timeutil::Time curTime = Global::GetInstance()->TimeTick;
		timeutil::Time timeElapsed = curTime - _lastTime;
		if (timeElapsed >= 10000)
		{
			_lastTime = curTime;
			float f = dis(eng);
			Vector3 pos{ _initPos.X + f, 0, _initPos.Z + f };
			//Vector3 pos{ 0, 0, 0 };
			pEnemy->SetPos(pos);
			std::cout << "random pos:" << pEnemy->GetPos().X << ", " << pEnemy->GetPos().Y << ", " << pEnemy->GetPos().Z << "\n";
		}
	}
};

