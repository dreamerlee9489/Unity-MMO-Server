#pragma once
#include "libserver/common.h"
#include "libserver/socket_object.h"
#include "libserver/entity.h"
#include "libserver/system.h"
#include "libserver/vector3.h"
#include "player_component_last_map.h"
#include "../../apps/space/player_component_detail.h"
#include "../../apps/space/npc.h"
#include "../../apps/game/team.h"
#include <algorithm>
#include <list>
#include <map>

struct Command
{
	int type = 0;
	uint64 target_sn = 0;
	Vector3 point{0, 0, 0};
};

class World;
class Npc;
class PlayerComponentLastMap;
class PlayerComponentDetail;
class Player : public Entity<Player>, public NetIdentify,
	virtual public IAwakeFromPoolSystem<NetIdentify*, std::string>,
	virtual public IAwakeFromPoolSystem<NetIdentify*, uint64, uint64>
{
public:
	PlayerComponentLastMap* lastMap = nullptr;
	PlayerComponentDetail* detail = nullptr;
	World* curWorld = nullptr;
	Team* pTeam = nullptr;
	Command cmd{};

	void Awake(NetIdentify* pIdentify, std::string account) override;
	void Awake(NetIdentify* pIdentify, uint64 playerSn, uint64 worldSn) override;
	void BackToPool() override;

	std::string GetAccount() const;
	std::string GetName() const;
	uint64 GetPlayerSN() const;
	Vector3& GetCurrPos() { return lastMap->GetCur()->Position; }
	void GetDamage(Npc* enemy);
	void ResetCmd();

	Proto::Player& GetPlayerProto();
	void ParseFromStream(uint64 playerSn, std::stringstream* pOpStream);
	void ParserFromProto(uint64 playerSn, const Proto::Player& proto);
	void SerializeToProto(Proto::Player* pProto);
	void Singlecast(Proto::MsgId msgId, google::protobuf::Message& proto, Player* pPlayer);
	void Broadcast(Proto::MsgId msgId, google::protobuf::Message& proto, std::map<uint64, Player*>& playerMap);

protected:
	uint64 _playerSn{ 0 };
	std::string _account{ "" };
	std::string _name{ "" };
	Proto::Player _player;
};

