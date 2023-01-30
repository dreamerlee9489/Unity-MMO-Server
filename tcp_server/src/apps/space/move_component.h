#pragma once
#include "libserver/component.h"
#include "libserver/system.h"
#include "libserver/vector3.h"
#include "npc.h"
#include <queue>

class Npc;
class PlayerComponentLastMap;
struct MoveVector3
{
	Vector3 Target = Vector3::Zero;
	float ScaleX{ 0 };
	float ScaleZ{ 0 };

	void SetTarget(Vector3 target, Vector3 position)
	{
		Target = target;
		const auto xdis = target.X - position.X;
		const auto zdis = target.Z - position.Z;
		const auto distance = sqrt(xdis * xdis + zdis * zdis);
		ScaleX = xdis / distance;
		ScaleZ = zdis / distance;
	}
};

class MoveComponent :public Component<MoveComponent>, public IAwakeFromPoolSystem<>
{
public:
	float moveSpeed = 1.56f;

	void Awake() override;
	void BackToPool() override;

	void SetPath(std::queue<Vector3> targets, Vector3 curPosition);
	bool Update(float timeElapsed, PlayerComponentLastMap* pLastMap, const float speed);
	bool Update(float timeElapsed, Npc* npc, const float speed);

private:
	std::queue<Vector3> _targets;
	MoveVector3 _vector3;
};
