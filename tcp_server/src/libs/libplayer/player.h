#pragma once
#include "libserver/common.h"
#include "libserver/socket_object.h"
#include "libserver/entity.h"
#include "libserver/system.h"
#include "libserver/vector3.h"
#include "player_component_last_map.h"

class Player : public Entity<Player>, public NetIdentify,
	virtual public IAwakeFromPoolSystem<NetIdentify*, std::string>,
	virtual public IAwakeFromPoolSystem<NetIdentify*, uint64, uint64>
{
public:
	PlayerComponentLastMap* lastMap;

	void Awake(NetIdentify* pIdentify, std::string account) override;
	void Awake(NetIdentify* pIdentify, uint64 playerSn, uint64 worldSn) override;
	void BackToPool() override;

	std::string GetAccount() const;
	std::string GetName() const;
	uint64 GetPlayerSN() const;
	int GetLv() const { return _lv; }
	int GetHp() const { return _hp; }
	int GetAtk() const { return _atk; }
	int GetDamage(int damage);
	Vector3& GetCurrPos() { return lastMap->GetCur()->Position; }

	Proto::Player& GetPlayerProto();
	void ParseFromStream(uint64 playerSn, std::stringstream* pOpStream);
	void ParserFromProto(uint64 playerSn, const Proto::Player& proto);
	void SerializeToProto(Proto::Player* pProto) const;

protected:
	uint64 _playerSn{ 0 };
	std::string _account{ "" };
	std::string _name{ "" };
	int _lv = 1, _hp = 1000, _atk = 10;
	Proto::Player _player;
};

