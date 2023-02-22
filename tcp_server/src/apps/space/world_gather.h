#pragma once
#include "libserver/entity.h"
#include "libserver/system.h"
#include "player_manager_component.h"
#include "space_team.h"

class Packet;
class World;
class SpaceTeam;
class PlayerManagerComponent;
class WorldGather :public Entity<WorldGather>, public IAwakeFromPoolSystem<>
{
public:
	std::map<uint64, SpaceTeam*> teamMap;

	void Awake() override;
	void BackToPool() override;
	void AddWorld(uint64 sn, World* world) { _worlds.emplace(sn, world); }
	void RegistPlayer(uint64 sn, Player* player);
	Player* GetPlayerBySn(uint64 sn);

private:
	void SyncSpaceInfo();
	void HandleCmdWorld(Packet* pPacket);
	void HandleWorldSyncToGather(Packet* pPacket);
	void HandleDungeonDisapper(Packet* pPacket);
	void HandleCreateTeam(Packet* pPacket);

private:
	// <map sn, map player count>
	std::map<uint64, int> _worldOnlines;
	std::map<uint64, World*> _worlds;
	std::map<uint64, Player*> _players;
};
