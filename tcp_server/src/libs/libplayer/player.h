#pragma once
#include "libserver/common.h"
#include "libserver/socket_object.h"
#include "libserver/entity.h"
#include "libserver/system.h"
#include "libserver/vector3.h"
#include "player_component_last_map.h"
#include "../../apps/space/player_component_detail.h"
#include "../../apps/space/ai_enemy.h"

class AIEnemy;
class PlayerComponentLastMap;
class PlayerComponentDetail;
class Player : public Entity<Player>, public NetIdentify,
	virtual public IAwakeFromPoolSystem<NetIdentify*, std::string>,
	virtual public IAwakeFromPoolSystem<NetIdentify*, uint64, uint64>
{
public:
	PlayerComponentLastMap* lastMap;
	PlayerComponentDetail* detail;

	void Awake(NetIdentify* pIdentify, std::string account) override;
	void Awake(NetIdentify* pIdentify, uint64 playerSn, uint64 worldSn) override;
	void BackToPool() override;

	std::string GetAccount() const;
	std::string GetName() const;
	uint64 GetPlayerSN() const;
	int GetDamage(AIEnemy* enemy);
	Vector3& GetCurrPos() { return lastMap->GetCur()->Position; }

	Proto::Player& GetPlayerProto();
	void ParseFromStream(uint64 playerSn, std::stringstream* pOpStream);
	void ParserFromProto(uint64 playerSn, const Proto::Player& proto);
	void SerializeToProto(Proto::Player* pProto);

protected:
	uint64 _playerSn{ 0 };
	std::string _account{ "" };
	std::string _name{ "" };
	Proto::Player _player;
};

