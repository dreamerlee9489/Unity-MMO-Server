#ifndef AI_ENEMY
#define AI_ENEMY
#include "libserver/entity.h"
#include "libserver/system.h"
#include "libserver/vector3.h"
#include "world.h"

class World;
class AIEnemy : public Entity<AIEnemy>, public IAwakeFromPoolSystem<int, Vector3>
{
	int _id = 0;
	float _viewDist = 6.0f, _atkDist = 1.5f;
	Vector3 _initPos;
	Vector3 _currPos;
	Vector3 _nextPos;
	World* _world = nullptr;
	Player* _linkPlayer = nullptr;
	std::map<uint64, Player*>* _players = nullptr;

public:
	void Awake(int id, Vector3 pos) override;

	void BackToPool() override;

	void SetWorld(World* const world) { _world = world; }

	void SetLinkPlayer(Player* player);

	void SetCurrPos(const Vector3& pos);

	void SetNextPos(const Vector3& pos);

	void SetPatrolPoint(int index);

	int GetID() const { return _id; }

	void SetAllPlayer(std::map<uint64, Player*>* players) { _players = players; }

	bool CanSee(Player* player);

	bool CanAttack(Player* player);

	Vector3& GetInitPos() { return _initPos; }

	Vector3& GetCurrPos() { return _currPos; }

	Vector3& GetNextPos() { return _nextPos; }

	World* GetWorld() const { return _world; }

	Player* GetLinkPlayer() const { return _linkPlayer; }

	std::map<uint64, Player*>* GetAllPlayer() { return _players; }
};

#endif // !AIENEMY
