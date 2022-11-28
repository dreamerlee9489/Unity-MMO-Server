﻿#pragma once
#include "libserver/system.h"
#include "libplayer/world_base.h"
#include "libserver/entity.h"
#include "libserver/socket_object.h"
#include "libresource/resource_manager.h"
#include "libresource/resource_help.h"
#include "libplayer/enemy.h"
#include <vector>

class Player;
class Enemy;
class World :public Entity<World>, public IWorld, public IAwakeFromPoolSystem<int>
{
public:
	void Awake(int worldId) override;
	void BackToPool() override;

protected:
	Player* GetPlayer(NetIdentify* pIdentify);

	void BroadcastPacket(Proto::MsgId msgId, google::protobuf::Message& proto);
	void BroadcastPacket(Proto::MsgId msgId, google::protobuf::Message& proto, std::set<uint64> players);

	void HandleNetworkDisconnect(Packet* pPacket);
	void HandleSyncPlayer(Packet* pPacket);
	void HandleRequestSyncPlayer(Player* pPlayer, Packet* pPacket);
	void HandleG2SRemovePlayer(Player* pPlayer, Packet* pPacket);
	void HandleMove(Player* pPlayer, Packet* pPacket);

private:
	void SyncWorldToGather();
	void SyncAppearTimer();
	void SyncEnemiesTimer();

private:
	// 缓存1秒内增加或是删除的玩家
	std::set<uint64> _addPlayer;
	std::vector<Enemy*> _enemies;
};

