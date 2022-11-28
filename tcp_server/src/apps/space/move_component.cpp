#include "move_component.h"
#include "libplayer/player.h"
#include "libplayer/player_component_last_map.h"

#include <cmath>

void MoveVector3::Update(Vector3 target, Vector3 position)
{
	Target = target;
	const auto xdis = target.X - position.X;
	const auto zdis = target.Z - position.Z;
	const auto distance = sqrt(xdis * xdis + zdis * zdis);

	ScaleX = xdis / distance;
	ScaleZ = zdis / distance;
}

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

void MoveComponent::Update(std::queue<Vector3> targets, Vector3 curPosition)
{
	if (targets.empty())
		return;

	// ɾ���ɵ�
	while (!_targets.empty())
	{
		_targets.pop();
	}

	std::swap(targets, _targets);

	auto v3 = _targets.front();
	_vector3.Update(v3, curPosition);
	_targets.pop();
}

bool MoveComponent::Update(const float timeElapsed, PlayerComponentLastMap* pLastMap, const float speed)
{
	auto curPosition = pLastMap->GetCur()->Position;

	// �����ƶ�����, timeElapsed Ϊ���룬�ٶ�����
	const auto moveDis = timeElapsed * 0.001 * speed;

	bool isStop = false;
	if (curPosition.GetDistance(_vector3.Target) < moveDis )
	{
		// �ѴﵽĿ���
		curPosition = _vector3.Target;

		// ������һ����
		if (!_targets.empty())
		{
			const auto v3 = _targets.front();
			_vector3.Update(v3, curPosition);
			_targets.pop();
		}
		else
		{
			isStop = true;
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
	LOG_DEBUG("cur position. " << curPosition);

	return isStop;
}
