﻿#ifndef AI_ENEMY
#define AI_ENEMY
#include "libserver/entity.h"
#include "libserver/system.h"
#include "libserver/vector3.h"
#include "world.h"

class World;
class AIEnemy : public Entity<AIEnemy>, public IAwakeFromPoolSystem<int, int, Vector3>
{
	int _id = 0;
	int _hp = 0;
	Vector3 _initPos;
	Vector3 _currPos;
	Vector3 _nextPos;
	World* _world = nullptr;
	Player* _linkPlayer = nullptr;
	std::map<uint64, Player*>* _players = nullptr;

public:
	void Awake(int id, int hp, Vector3 pos) override;

	void BackToPool() override;

	void SetWorld(World* const world) { _world = world; }

	void SetLinkPlayer(Player* player);

	void SetCurrPos(const Vector3& pos);

	void SetNextPos(const Vector3& pos);

	void SetPatrolPoint(int index);

	int GetID() const { return _id; }

	int GetHP() const { return _hp; }

	void SetAllPlayer(std::map<uint64, Player*>* players) { _players = players; }

	Vector3& GetInitPos() { return _initPos; }

	Vector3& GetCurrPos() { return _currPos; }

	Vector3& GetNextPos() { return _nextPos; }

	World* GetWorld() const { return _world; }

	Player* GetLinkPlayer() const { return _linkPlayer; }

	std::map<uint64, Player*>* GetAllPlayer() { return _players; }
};

#endif // !AIENEMY