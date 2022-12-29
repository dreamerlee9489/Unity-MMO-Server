#pragma once
#include "libserver/system.h"
#include "libplayer/world_base.h"
#include "libserver/entity.h"
#include "libserver/socket_object.h"
#include "libresource/resource_manager.h"
#include "libresource/resource_help.h"
#include "libserver/message_system_help.h"
#include "libserver/message_system.h"
#include "libplayer/player_component_last_map.h"
#include "player_manager_component.h"
#include "player_component_detail.h"
#include "move_component.h"
#include "fsm_component.h"
#include "ai_enemy.h"
#include <vector>
#include <cfloat>

class Player;
class AIEnemy;
class World :public Entity<World>, public IWorld, public IAwakeFromPoolSystem<int>
{
public:
	void Awake(int worldId) override;
	void BackToPool() override;
	void BroadcastPacket(Proto::MsgId msgId, google::protobuf::Message& proto);
	void BroadcastPacket(Proto::MsgId msgId, google::protobuf::Message& proto, std::set<uint64> players);
	Player* GetNearestPlayer(Vector3& pos);
	PlayerManagerComponent* GetPlayerManager() const { return _playerManager; }

protected:
	Player* GetPlayer(NetIdentify* pIdentify);
	void HandleNetworkDisconnect(Packet* pPacket);
	void HandleSyncPlayer(Packet* pPacket);
	void HandleFsmSyncState(Packet* pPacket);
	void HandleEnemy(Packet* pPacket);
	void HandleRequestSyncPlayer(Player* pPlayer, Packet* pPacket);
	void HandleG2SRemovePlayer(Player* pPlayer, Packet* pPacket);
	void HandleMove(Player* pPlayer, Packet* pPacket);
	void HandlePlayerSyncState(Player* pPlayer, Packet* pPacket);
	void HandleRequestSyncEnemy(Player* pPlayer, Packet* pPacket);

private:
	void SyncWorldToGather();
	void SyncAppearTimer();

	// 缓存1秒内增加或是删除的玩家
	std::set<uint64> _addPlayer;
	std::vector<AIEnemy*> _enemies;
	PlayerManagerComponent* _playerManager;
};

