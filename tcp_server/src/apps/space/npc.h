#ifndef AI_ENEMY
#define AI_ENEMY
#include "libserver/entity.h"
#include "libserver/system.h"
#include "libserver/vector3.h"
#include "libresource/resource_world.h"
#include "world.h"
#include <list>
#include <random>
#include <algorithm>
constexpr int DROP_EXPR = -1;
constexpr int DROP_GOLD = -2;

enum struct ItemType { None, Potion, Weapon };
enum struct KnapType { World, Bag, Equip, Action, Trade };

struct ItemData
{
	uint64 sn = 0;
	ItemType itemType = ItemType::None;
	KnapType knapType = KnapType::World;
	int id = 0, index = 0;

	ItemData() = default;

	ItemData(ItemType itemType, int id, KnapType knapType, int index = 0, uint64 sn = 0)
	{
		this->itemType = itemType;
		this->knapType = knapType;
		this->id = id;
		this->index = index;
		this->sn = sn == 0 ? Global::GetInstance()->GenerateSN() : sn;
	}

	void SerializeToProto(Proto::ItemData* proto)
	{
		proto->set_itemtype((Proto::ItemData_ItemType)itemType);
		proto->set_knaptype((Proto::ItemData_KnapType)knapType);
		proto->set_sn(sn);
		proto->set_id(id);
		proto->set_index(index);
	}

	const ItemData& ParserFromProto(const Proto::ItemData& proto)
	{
		itemType = (ItemType)proto.itemtype();
		knapType = (KnapType)proto.knaptype();
		sn = proto.sn();
		id = proto.id();
		index = proto.index();
		return *this;
	}
};

class World;
class Player;
class Npc : public Entity<Npc>, public IAwakeFromPoolSystem<ResourceNpc>
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

	void Awake(ResourceNpc cfg) override;

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

	std::list<ItemData>* GetDropList(Player* player);
};

#endif // !AIENEMY
