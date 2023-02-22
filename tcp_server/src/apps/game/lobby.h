﻿#pragma once
#include "libplayer/world_base.h"
#include "libserver/system.h"
#include "libserver/entity.h"
#include "libserver/socket_object.h"

class Packet;
class Player;
class Lobby : public Entity<Lobby>, public IWorld, public IAwakeSystem<>
{
public:
	void Awake() override;
	void BackToPool() override;

private:
	Player* GetPlayer(NetIdentify* pIdentify);
	void HandleNetworkDisconnect(Packet* pPacket);
	void HandleLoginByToken(Packet* pPacket);
	void HandleGameTokenToRedisRs(Packet* pPacket);
	void HandleQueryPlayerRs(Packet* pPacket);
	void HandleQueryWorldRs(Packet* pPacket);
	void HandleBroadcastCreateWorldProxy(Packet* pPacket);
	void HandleTeleportAfter(Player* pPlayer, Packet* pPacket);

	// 进入公共地图
	void EnterPublicWorld(Player* pPlayer);

private:
	/// <summary>
	/// 等待跳转到公共地图的玩家集合
	/// </summary>
	std::map<int, std::set<uint64>> _waitingForWorld;
	/// <summary>
	/// 等待跳转到副本地图的玩家集合
	/// </summary>
	std::map<uint64, std::set<uint64>> _waitingForDungeon;
};

