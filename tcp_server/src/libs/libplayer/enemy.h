#pragma once
#include "libserver/common.h"
#include "libserver/socket_object.h"
#include "libserver/entity.h"
#include "libserver/system.h"
#include "libserver/vector3.h"

class Enemy : public Entity<Enemy>, public IAwakeFromPoolSystem<int, int, Vector3>
{
public:
	void Awake(int id, int hp, Vector3 pos) override
	{
		_id = id;
		_hp = hp;
		_pos = pos;
	}

	void BackToPool() override
	{
		_id = 0;
		_hp = 0;
		_pos = { 0, 0, 0 };
	}

	int GetID() const { return _id; }
	int GetHP() const { return _hp; }
	Vector3 GetPos() const { return _pos; }
	void SetPos(Vector3& pos)
	{
		_pos.X = pos.X;
		_pos.Y = pos.Y;
		_pos.Z = pos.Z;
	}

	//void ParserFromProto(uint64 enemySn, const Proto::Enemy& proto);
	//void SerializeToProto(Proto::Enemy* pProto) const;

protected:
	int _id = 0;
	int _hp = 0;
	Vector3 _pos{ 0, 0, 0 };
};

