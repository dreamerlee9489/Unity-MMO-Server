#include "world_component_teleport.h"
#include "world_proxy_locator.h"
#include "world_proxy_help.h"
#include "teleport_object.h"
#include "world_proxy.h"

#include "libserver/message_system_help.h"
#include "libserver/sync_component.h"

#include "libresource/resource_world.h"
#include "libresource/resource_manager.h"
#include "libresource/resource_help.h"

#include "libplayer/player.h"
#include "libplayer/player_collector_component.h"
#include "space_sync_handler.h"
#include "libserver/component_help.h"

void WorldComponentTeleport::Awake()
{
}

void WorldComponentTeleport::BackToPool()
{
    if (!_objects.empty())
        LOG_WARN("not completed to teleport. WorldComponentTeleport be destroyed.");

    for (const auto& pair : _objects)
        GetSystemManager()->GetEntitySystem()->RemoveComponent(pair.second);
    _objects.clear();
}

bool WorldComponentTeleport::IsTeleporting(Player* pPlayer)
{
    return _objects.find(pPlayer->GetPlayerSN()) != _objects.end();
}

void WorldComponentTeleport::CreateTeleportObject(int worldId, Player* pPlayer)
{
    const auto pObj = GetSystemManager()->GetEntitySystem()->AddComponent<TeleportObject>(worldId, pPlayer->GetPlayerSN());
    _objects.insert(std::make_pair(pPlayer->GetPlayerSN(), pObj));
    const auto pWorldProxy = GetParent<WorldProxy>();
    // World
    CreateWorldFlag(pWorldProxy, worldId, pObj);
    // Sync
    CreateSyncFlag(pWorldProxy, pObj);
}

void WorldComponentTeleport::CreateWorldFlag(WorldProxy* pWorldProxy, int targetWorldId, TeleportObject* pObj)
{
    const auto pResMgr = ResourceHelp::GetResourceManager();
    const auto pWorldRes = pResMgr->Worlds->GetResource(targetWorldId);

    if (pWorldRes->IsType(ResourceWorldType::Public))
    {
        auto pWorldLocator = ComponentHelp::GetGlobalEntitySystem()->GetComponent<WorldProxyLocator>();
        const auto worldSn = pWorldLocator->GetWorldSnById(targetWorldId);
        if (worldSn == static_cast<uint64>(INVALID_ID))
        {
            Proto::RequestWorld protoToMgr;
            protoToMgr.set_world_id(targetWorldId);
            MessageSystemHelp::SendPacket(Proto::MsgId::G2M_RequestWorld, protoToMgr, APP_APPMGR);
            pObj->FlagWorld.Flag = TeleportFlagType::Waiting;
        }
        else
        {
            pObj->FlagWorld.SetValue(worldSn);
        }
    }
    else if (pWorldRes->IsType(ResourceWorldType::Dungeon))
    {
        auto pSpaceSyncHandler = ComponentHelp::GetGlobalEntitySystem()->GetComponent<SpaceSyncHandler>();
        AppInfo info;
        if (!pSpaceSyncHandler->GetSpaceApp(&info))
        {
            LOG_ERROR("can't find space");
            return;
        }

        Proto::CreateWorld protoCreate;
        protoCreate.set_world_id(targetWorldId);
        protoCreate.set_last_world_sn(pWorldProxy->GetSN());
        protoCreate.set_game_app_id(Global::GetInstance()->GetCurAppId());
        MessageSystemHelp::SendPacket(Proto::MsgId::G2S_CreateWorld, protoCreate, APP_SPACE, info.AppId);

        pObj->FlagWorld.Flag = TeleportFlagType::Waiting;
    }
    else
    {
        LOG_ERROR("WorldComponentTeleport");
    }
}

void WorldComponentTeleport::CreateSyncFlag(WorldProxy* pWorldProxy, TeleportObject* pObj)
{
    auto pPlayerMgr = _parent->GetComponent<PlayerCollectorComponent>();
    const auto pPlayer = pPlayerMgr->GetPlayerBySn(pObj->GetPlayerSN());

    Proto::RequestSyncPlayer protoSync;
    protoSync.set_player_sn(pObj->GetPlayerSN());
    pWorldProxy->SendPacketToWorld(Proto::MsgId::G2S_RequestSyncPlayer, protoSync, pPlayer);

    pObj->FlagPlayerSync.Flag = TeleportFlagType::Waiting;
}

void WorldComponentTeleport::HandleBroadcastCreateWorldProxy(const int worldId, const uint64 worldSn)
{
    const auto pResMgr = ResourceHelp::GetResourceManager();
    const auto pWorldRes = pResMgr->Worlds->GetResource(worldId);
    auto& teamMap = GetParent<WorldProxy>()->proxyMgr->teamMap;
    if (pWorldRes->IsType(ResourceWorldType::Public))
    {
        do
        {
            auto iter = std::find_if(_objects.begin(), _objects.end(), [&worldId](auto pair)
                {
                    return (pair.second->GetTargetWorldId() == worldId);
                });

            if (iter == _objects.end())
                break;

            auto pObj = iter->second;
            pObj->FlagWorld.SetValue(worldSn);
            Check(pObj);

        } while (true);
    }
    else if (pWorldRes->IsType(ResourceWorldType::Dungeon))
    {
        auto iter = std::find_if(_objects.begin(), _objects.end(), [&worldId](auto pair)
            {
                return (pair.second->GetTargetWorldId() == worldId);
            });

        if (iter == _objects.end())
        {
            LOG_ERROR("BroadcastCreateWorldProxy, can't find teleport object. create world id:" << worldId << " cur world id:" << GetParent<WorldProxy>()->GetWorldId());
            return;
        }

        if (teamMap.find(iter->first) != teamMap.end())
        {
            teamMap[iter->first]->dungeonSn = worldSn;
            teamMap[iter->first]->dungeonId = worldId;
        }

        auto pObj = iter->second;
        pObj->FlagWorld.SetValue(worldSn);
        Check(pObj);
    }
}

void WorldComponentTeleport::BroadcastSyncPlayer(uint64 playerSn)
{
    auto iter = std::find_if(_objects.begin(), _objects.end(), [&playerSn](auto pair)
        {
            return pair.second->GetPlayerSN() == playerSn;
        });

    if (iter == _objects.end())
        return;

    auto pObj = iter->second;
    pObj->FlagPlayerSync.SetValue(true);
    Check(pObj);
}

bool WorldComponentTeleport::Check(TeleportObject* pObj)
{
    const auto worldId = pObj->GetTargetWorldId();
    const auto pWorldProxy = GetParent<WorldProxy>();

    if (!pObj->FlagPlayerSync.IsCompleted() || !pObj->FlagWorld.IsCompleted())
        return false;

    auto pPlayerMgr = _parent->GetComponent<PlayerCollectorComponent>();
    const auto pPlayer = pPlayerMgr->GetPlayerBySn(pObj->GetPlayerSN());

    if (pObj->FlagWorld.GetValue() == 0)
    {
        LOG_ERROR("error. can't find targe world proxy's obj. world id:" << worldId);
        return false;
    }

    WorldProxyHelp::Teleport(pPlayer, pWorldProxy->GetSN(), pObj->FlagWorld.GetValue());
    if (pWorldProxy->proxyMgr->teamMap.find(pPlayer->GetPlayerSN()) != pWorldProxy->proxyMgr->teamMap.end())
    {
        Team* pTeam = pWorldProxy->proxyMgr->teamMap[pPlayer->GetPlayerSN()];
        if (pTeam->dungeonId == worldId && pWorldProxy->proxyLoc->IsExistDungeon(pObj->FlagWorld.GetValue()))
        {
            for (uint64 sn : pTeam->GetMembers())
            {
                if (sn != pPlayer->GetPlayerSN())
                {
                    Proto::EnterDungeon proto;
                    proto.set_world_id(worldId);
                    proto.set_world_sn(pObj->FlagWorld.GetValue());
                    proto.set_sender(pPlayer->GetName());
                    MessageSystemHelp::SendPacket(Proto::MsgId::C2C_ReqEnterDungeon, proto, pWorldProxy->proxyMgr->playerMgr->GetPlayerBySn(sn));
                }
            }
        }
    }

    _objects.erase(pPlayer->GetPlayerSN());
    GetSystemManager()->GetEntitySystem()->RemoveComponent(pObj);  
    return true;
}