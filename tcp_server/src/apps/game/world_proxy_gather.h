#pragma once
#include "libplayer/player_collector_component.h"
#include "libplayer/player.h"
#include "libserver/system.h"
#include "libserver/entity.h"
#include "game_team.h"

class Packet;
class Player;
class PlayerCollectorComponent;
class GameTeam;
struct WorldProxyInfo
{
	uint64 WorldSn;
	int WorldId;
	int Online;
};

class WorldProxyGather :public Entity<WorldProxyGather>, public IAwakeSystem<>
{
public:
	PlayerCollectorComponent* playerMgr = nullptr;
	std::map<uint64, GameTeam*> teamMap;

	void Awake() override;
	void BackToPool() override;

private:
	void SyncGameInfo();
	void HandleWorldProxySyncToGather(Packet* pPacket);
	void HandleCmdWorldProxy(Packet* pPacket);
	void HandleNetworkDisconnect(Packet* pPacket);
	void HandleLoginByToken(Packet* pPacket);
	void HandleQueryPlayerRs(Packet* pPacket);
	void HandleCreateTeam(Packet* pPacket);

private:
	std::map<uint64, WorldProxyInfo> _maps;
};
