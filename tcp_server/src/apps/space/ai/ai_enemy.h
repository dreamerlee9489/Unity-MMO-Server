#ifndef AI_ENEMY
#define AI_ENEMY
#include "libserver/entity.h"
#include "libserver/system.h"
#include "libserver/vector3.h"
#include "../world.h"

class World;
class AIEnemy : public Entity<AIEnemy>, public IAwakeFromPoolSystem<int, int, Vector3>
{
	std::default_random_engine _eng;
	std::uniform_real_distribution<float> _dis = std::uniform_real_distribution<float>(-6.0, 6.0);
	Vector3 _initPos;

protected:
	int _id = 0;
	int _hp = 0;
	Vector3 _pos;
	World* _world;

public:
	int GetID() const { return _id; }
	int GetHP() const { return _hp; }
	Vector3 GetPos() const { return _pos; }
	World* GetWorld() const { return _world; }

	void Awake(int id, int hp, Vector3 pos) override
	{
		_id = id;
		_hp = hp;
		_pos = pos;
		_initPos = pos;
		_eng = std::default_random_engine(id);
		SetRandPos();
	}

	void BackToPool() override
	{
		_id = 0;
		_hp = 0;
		_pos = { 0, 0, 0 };
	}

	void SetPos(Vector3& pos)
	{
		_pos.X = pos.X;
		_pos.Y = pos.Y;
		_pos.Z = pos.Z;
	}

	void SetRandPos()
	{
		float fx = _dis(_eng), fz = _dis(_eng);
		_pos.X = _initPos.X + fx;
		_pos.Z = _initPos.Z + fz;
	}

	void SetWorld(World* world) { _world = world; }
};

#endif // !AIENEMY
