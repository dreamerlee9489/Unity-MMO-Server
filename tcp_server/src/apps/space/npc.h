#ifndef AI_ENEMY
#define AI_ENEMY
#include "libserver/entity.h"
#include "libserver/system.h"
#include "libserver/vector3.h"
#include "libresource/resource_world.h"
#include "world.h"
#include <vector>
#include <random>
#include <algorithm>
constexpr int DROP_EXPR = -1;
constexpr int DROP_GOLD = -2;

enum struct ItemType { None, Potion, Weapon };

struct ItemData
{
	uint64 sn = 0;
	ItemType type = ItemType::None;
	int id = 0, index = 0;

	ItemData(ItemType type, int id, int index = 0, uint64 sn = 0)
	{
		this->sn = sn == 0 ? Global::GetInstance()->GenerateSN() : sn;
		this->type = type;
		this->id = id;
		this->index = index;
	}

	void SerializeToProto(Proto::ItemData* proto)
	{
		switch (type)
		{
		case ItemType::Potion:
			proto->set_type(Proto::ItemData_ItemType_Potion);
			break;
		case ItemType::Weapon:
			proto->set_type(Proto::ItemData_ItemType_Weapon);
			break;
		default:
			proto->set_type(Proto::ItemData_ItemType_None);
			break;
		}
		proto->set_sn(sn);
		proto->set_id(id);
		proto->set_index(index);
	}

	void ParserFromProto(const Proto::ItemData& proto)
	{
		switch ((Proto::ItemData_ItemType)proto.type())
		{
		case Proto::ItemData_ItemType_Potion:
			type = ItemType::Potion;
			break;
		case Proto::ItemData_ItemType_Weapon:
			type = ItemType::Weapon;
			break;
		default:
			type = ItemType::None;
			break;
		}
		sn = proto.sn();
		id = proto.id();
		index = proto.index();
	}
};

class World;
class Player;
class Npc : public Entity<Npc>, public IAwakeFromPoolSystem<ResourceNpc&>
{
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
	int id = 0, type = 0, lv = 0, hp = 0, atk = 0, def = 0;

	void Awake(ResourceNpc& cfg) override;

	void BackToPool() override;

	void SetWorld(World* const world) { _world = world; }

	void SetLinkPlayer(Player* player);

	void SetCurrPos(const Vector3& pos);

	void SetNextPos(const Vector3& pos);

	void SetPatrolPoint(int index);

	void SetAllPlayer(std::map<uint64, Player*>* players) { _players = players; }

	bool CanSee(Player* player);

	bool CanAttack(Player* player);

	void GetDamage(Player* attacker);

	const int GetID() const { return id; }

	Vector3& GetInitPos() { return _initPos; }

	Vector3& GetCurrPos() { return _currPos; }

	Vector3& GetNextPos() { return _nextPos; }

	World* GetWorld() const { return _world; }

	Player* GetLinkPlayer() const { return _linkPlayer; }

	std::map<uint64, Player*>* GetAllPlayer() { return _players; }

	std::vector<ItemData>* GetDropList(Player* player);
};

#endif // !AIENEMY
