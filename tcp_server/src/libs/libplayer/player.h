﻿#pragma once
#include "libserver/common.h"
#include "libserver/socket_object.h"
#include "libserver/entity.h"
#include "libserver/system.h"
#include "libserver/vector3.h"
#include "space/player_component_detail.h"
#include "space/npc.h"
#include "game/game_team.h"
#include "player_component_last_map.h"
#include "cmd_component.h"
#include <algorithm>
#include <list>
#include <map>

class World;
class Npc;
class PlayerComponentLastMap;
class PlayerComponentDetail;
class Player : public Entity<Player>, public NetIdentify, virtual public IAwakeFromPoolSystem<NetIdentify*, std::string>, virtual public IAwakeFromPoolSystem<NetIdentify*, uint64, uint64>
{
	float _viewDist = 8.0f, _atkDist = 2.0f;

public:
	PlayerComponentLastMap* lastMap = nullptr;
	PlayerComponentDetail* detail = nullptr;
	World* curWorld = nullptr;

	void Awake(NetIdentify* pIdentify, std::string account) override;
	void Awake(NetIdentify* pIdentify, uint64 playerSn, uint64 worldSn) override;
	void BackToPool() override;

	int GetSocket() const { return _socketKey.Socket; }
	std::string GetAccount() const { return _account; }
	std::string GetName() const { return _name; }
	uint64 GetPlayerSN() const { return _playerSn; }
	Vector3& GetCurrPos() { return lastMap->GetCur()->Position; }
	Proto::Player& GetPlayerProto() { return _player; }

	void GetDamage(Npc* enemy);
	void GetDamage(Player* atker);
	void UpdateKnapItem(const Proto::ItemData& itemData);
	void GetPlayerKnap();
	bool CanSee(Vector3& point);
	bool CanAttack(Vector3& point);

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

