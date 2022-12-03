#include "ai_enemy.h"

void AIEnemy::Awake(int id, int hp, Vector3 pos)
{
	_id = id;
	_hp = hp;
	_initPos = pos;
	_currPos = pos;
	_nextPos = pos;
	_speed = WalkSpeed;
}

void AIEnemy::BackToPool()
{
	_id = 0;
	_hp = 0;
	_nextPos = { 0, 0, 0 };
}

void AIEnemy::SetCurrPos(const Vector3& pos)
{
	_currPos.X = pos.X;
	_currPos.Y = pos.Y;
	_currPos.Z = pos.Z;
}

void AIEnemy::SetNextPos(const Vector3& pos)
{
	_nextPos.X = pos.X;
	_nextPos.Y = pos.Y;
	_nextPos.Z = pos.Z;
}

void AIEnemy::SetPatrolPoint(int index)
{
	switch (index)
	{
	case 0:
		_nextPos.X = _initPos.X - 5;
		_nextPos.Z = _initPos.Z - 5;
		break;
	case 1:
		_nextPos.X = _initPos.X - 5;
		_nextPos.Z = _initPos.Z + 5;
		break;
	case 2:
		_nextPos.X = _initPos.X + 5;
		_nextPos.Z = _initPos.Z - 5;
		break;
	case 3:
		_nextPos.X = _initPos.X + 5;
		_nextPos.Z = _initPos.Z + 5;
		break;
	}
}

void AIEnemy::UpdatePos(uint64 timeElapsed)
{
	_dist = _currPos.GetDistance(_nextPos);
	if (_dist > StopDist)
	{
		float deltaTime = timeElapsed * 1.0 / 1000;
		Vector3 dir = (_nextPos - _currPos).Normalize();
		_currPos.X += deltaTime * _speed * dir.X;
		_currPos.Z += deltaTime * _speed * dir.Z;
	}
}
