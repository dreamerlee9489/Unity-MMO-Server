#ifndef AI_ENEMY
#define AI_ENEMY
#include "libserver/entity.h"
#include "libserver/system.h"
#include "libserver/vector3.h"
#include "../world.h"

class World;
class AIEnemy : public Entity<AIEnemy>, public IAwakeFromPoolSystem<int, int, Vector3>
{
	int _id = 0;
	int _hp = 0;
	float _speed = 0;
	float _dist = 0;
	Vector3 _initPos;
	Vector3 _currPos;
	Vector3 _nextPos;
	World* _world;

public:
	const float WalkSpeed = 1.56f;
	const float RunSpeed = 5.56f;
	const float StopDist = 1.0f;
	const float ViewDist = 6.0f;
	const float PursuitDist = 4.0f;
	const float AttackDist = 1.5f;

	void Awake(int id, int hp, Vector3 pos) override;

	void BackToPool() override;

	int GetID() const { return _id; }

	int GetHP() const { return _hp; }

	float GetCurrSpeed() const { return _speed; }

	float GetDistToNext() const { return _dist; }

	Vector3 GetInitPos() const { return _initPos; }

	Vector3 GetCurrPos() const { return _currPos; }

	Vector3 GetNextPos() const { return _nextPos; }

	World* GetWorld() const { return _world; }

	void SetWorld(World* const world) { _world = world; }

	void SetSpeed(const float speed) { _speed = speed; }

	void SetCurrPos(const Vector3& pos);

	void SetNextPos(const Vector3& pos);

	void SetPatrolPoint(int index);

	void UpdatePos(uint64 timeElapsed);
};

#endif // !AIENEMY
