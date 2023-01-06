#ifndef AI_ENEMY
#define AI_ENEMY
#include "libserver/entity.h"
#include "libserver/system.h"
#include "libserver/vector3.h"
#include "libresource/resource_world.h"
#include "world.h"
#include <vector>
#include <random>
constexpr int DROP_EXPR = -1;
constexpr int DROP_GOLD = -2;

enum struct ItemType { None, Potion, Weapon };

struct DropItem
{
	int id = 0, num = 0, index = 0;
	std::string key;
	ItemType type = ItemType::None;

	DropItem(ItemType type, int id, int num, int index, std::string key = "")
	{
		this->type = type;
		this->id = id;
		this->num = num;
		this->index = index;
		this->key = std::move(key);
	}
};

class World;
class Player;
class AIEnemy : public Entity<AIEnemy>, public IAwakeFromPoolSystem<ResourceEnemy&>
{
	int _id = 0;
	float _viewDist = 8.0f, _atkDist = 1.5f;
	Vector3 _initPos;
	Vector3 _currPos;
	Vector3 _nextPos;
	World* _world = nullptr;
	Player* _linkPlayer = nullptr;
	std::map<uint64, Player*>* _players = nullptr;

	timeutil::Time _currTime;
	std::default_random_engine _intEng;
	std::uniform_int_distribution<int> _idDis, _numDis;
	static std::default_random_engine _realEng;
	static std::uniform_real_distribution<double> _realDis;

public:
	int lv = 0, hp = 0, atk = 0, def = 0;

	void Awake(ResourceEnemy& cfg) override;

	void BackToPool() override;

	void SetWorld(World* const world) { _world = world; }

	void SetLinkPlayer(Player* player);

	void SetCurrPos(const Vector3& pos);

	void SetNextPos(const Vector3& pos);

	void SetPatrolPoint(int index);

	void SetAllPlayer(std::map<uint64, Player*>* players) { _players = players; }

	bool CanSee(Player* player);

	bool CanAttack(Player* player);

	int GetDamage(Player* attacker);

	int GetID() { return _id; }

	std::vector<DropItem>* GetDropList(Player* player);

	Vector3& GetInitPos() { return _initPos; }

	Vector3& GetCurrPos() { return _currPos; }

	Vector3& GetNextPos() { return _nextPos; }

	World* GetWorld() const { return _world; }

	Player* GetLinkPlayer() const { return _linkPlayer; }

	std::map<uint64, Player*>* GetAllPlayer() { return _players; }
};

#endif // !AIENEMY
