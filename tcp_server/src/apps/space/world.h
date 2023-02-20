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
#include "libplayer/command_component.h"
#include "player_manager_component.h"
#include "player_component_detail.h"
#include "move_component.h"
#include "bt_component.h"
#include "trade.h"
#include <vector>
#include <unordered_map>

class Player;
class Npc;
class PlayerManagerComponent;
class World :public Entity<World>, public IWorld, public IAwakeFromPoolSystem<int>
{
public:
	std::vector<int>* potionCfgs;
	std::vector<int>* weaponCfgs;
	std::vector<Npc*> npcs;
	std::unordered_map<uint64, int> npcIdxMap;
	PlayerManagerComponent* playerMgr;

	void Awake(int worldId) override;
	void BackToPool() override;
	void BroadcastPacket(Proto::MsgId msgId, google::protobuf::Message& proto);
	void BroadcastPacket(Proto::MsgId msgId, google::protobuf::Message& proto, std::set<uint64>& players);
	Player* GetNearestPlayer(Vector3& pos);
	PlayerManagerComponent* GetPlayerManager() const { return playerMgr; }

protected:
	Player* GetPlayer(NetIdentify* pIdentify);
	void HandleNetworkDisconnect(Packet* pPacket);
	void HandleSyncPlayer(Packet* pPacket);
	void HandleSyncNpcPos(Packet* pPacket);
	void HandlePlayerMove(Player* pPlayer, Packet* pPacket);
	void HandleNpcMove(Player* pPlayer, Packet* pPacket);
	void HandlePlayerAtkEvent(Player* pPlayer, Packet* pPacket);
	void HandleNpcAtkEvent(Player* pPlayer, Packet* pPacket);
	void HandleRequestSyncPlayer(Player* pPlayer, Packet* pPacket);
	void HandleG2SRemovePlayer(Player* pPlayer, Packet* pPacket);
	void HandleReqNpcInfo(Player* pPlayer, Packet* pPacket);
	void HandleSyncBtAction(Player* pPlayer, Packet* pPacket);
	void HandleReqSyncPlayer(Player* pPlayer, Packet* pPacket);
	void HandleSyncPlayerPos(Player* pPlayer, Packet* pPacket);
	void HandleSyncPlayerCmd(Player* pPlayer, Packet* pPacket);
	void HandleUpdateKnapItem(Player* pPlayer, Packet* pPacket);
	void HandleUpdateKnapGold(Player* pPlayer, Packet* pPacket);
	void HandleGetPlayerKnap(Player* pPlayer, Packet* pPacket);
	void HandleReqTrade(Player* pPlayer, Packet* pPacket);
	void HandleTradeRes(Player* pPlayer, Packet* pPacket);
	void HandleUpdateTradeItem(Player* pPlayer, Packet* pPacket);

private:
	void SyncWorldToGather();
	void SyncAppearTimer();
	void PlayerDisappear(Player* pPlayer);

	// 缓存1秒内增加或是删除的玩家
	std::set<uint64> _adds;
	std::unordered_map<uint64, Trade*> tradeMap;
};

