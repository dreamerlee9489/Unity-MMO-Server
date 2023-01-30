#include "move_component.h"
#include "libplayer/player.h"
#include "libplayer/player_component_last_map.h"
#include <cmath>

void MoveComponent::Awake()
{
	while (!_targets.empty())
		_targets.pop();
}

void MoveComponent::BackToPool()
{
	while (!_targets.empty())
		_targets.pop();
}

void MoveComponent::SetPath(std::queue<Vector3> targets, Vector3 curPosition)
{
	if (targets.empty())
		return;
	// 删掉旧点
	while (!_targets.empty())
		_targets.pop();

	std::swap(targets, _targets);
	auto& v3 = _targets.front();
	_vector3.SetTarget(v3, curPosition);
	_targets.pop();
}

bool MoveComponent::Update(const float timeElapsed, PlayerComponentLastMap* pLastMap, const float speed)
{
	auto curPosition = pLastMap->GetCur()->Position;
	// 本次移动距离, timeElapsed 为毫秒，速度是秒
	const auto moveDis = timeElapsed * 0.001 * speed;

	bool isStop = false;
	if (curPosition.GetDistance(_vector3.Target) < moveDis)
	{
		// 已达到目标点
		curPosition = _vector3.Target;
		// 计算下一个点
		if (_targets.empty())
			isStop = true;
		else
		{
			const auto& v3 = _targets.front();
			_vector3.SetTarget(v3, curPosition);
			_targets.pop();
		}
	}
	else
	{
		const auto xDis = moveDis * _vector3.ScaleX;
		const auto zDis = moveDis * _vector3.ScaleZ;

		curPosition.X += xDis;
		curPosition.Z += zDis;
	}

	pLastMap->GetCur()->Position = curPosition;
	//LOG_DEBUG("cur position. " << curPosition);
	return isStop;
}

bool MoveComponent::Update(float timeElapsed, Npc* npc, const float speed)
{
	auto& curPosition = npc->GetCurrPos();
	// 本次移动距离, timeElapsed 为毫秒，速度是秒
	const auto moveDis = timeElapsed * 0.001 * speed;

	bool isStop = false;
	if (curPosition.GetDistance(_vector3.Target) < moveDis)
	{
		// 已达到目标点
		curPosition = _vector3.Target;
		// 计算下一个点
		if (_targets.empty())
			isStop = true;
		else
		{
			const auto& v3 = _targets.front();
			_vector3.SetTarget(v3, curPosition);
			_targets.pop();
		}
	}
	else
	{
		const auto xDis = moveDis * _vector3.ScaleX;
		const auto zDis = moveDis * _vector3.ScaleZ;

		curPosition.X += xDis;
		curPosition.Z += zDis;
	}

	npc->SetCurrPos(curPosition);
	//LOG_DEBUG("npc cur position. " << curPosition << " speed=" << speed);
	return isStop;
}
