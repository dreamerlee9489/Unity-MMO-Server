#include "world_proxy_gather.h"

#include "libserver/message_system_help.h"
#include "libserver/thread_mgr.h"
#include "libserver/message_system.h"
#include "libserver/global.h"

#include <numeric>
#include "libresource/resource_help.h"

void WorldProxyGather::Awake()
{
    AddTimer(0, 10, true, 2, BindFunP0(this, &WorldProxyGather::SyncGameInfo));
    playerMgr = AddComponent<PlayerCollectorComponent>();
    // message
    auto pMsgSystem = GetSystemManager()->GetMessageSystem();
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_NetworkDisconnect, BindFunP1(this, &WorldProxyGather::HandleNetworkDisconnect));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_WorldProxySyncToGather, BindFunP1(this, &WorldProxyGather::HandleWorldProxySyncToGather));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_CmdWorldProxy, BindFunP1(this, &WorldProxyGather::HandleCmdWorldProxy));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::C2G_LoginByToken, BindFunP1(this, &WorldProxyGather::HandleLoginByToken));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::G2DB_QueryPlayerRs, BindFunP1(this, &WorldProxyGather::HandleQueryPlayerRs));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_CreateTeam, BindFunP1(this, &WorldProxyGather::HandleCreateTeam));
}

void WorldProxyGather::BackToPool()
{
    _maps.clear();
    for (auto& pair : teamMap)
        delete pair.second;
}

void WorldProxyGather::SyncGameInfo()
{
    Proto::AppInfoSync proto;

    const int online = std::accumulate(_maps.begin(), _maps.end(), 0, [](int value, auto pair)
        {
            return value + pair.second.Online;
        });

    proto.set_app_id(Global::GetInstance()->GetCurAppId());
    proto.set_app_type((int)Global::GetInstance()->GetCurAppType());
    proto.set_online(online);

    MessageSystemHelp::SendPacket(Proto::MsgId::MI_AppInfoSync, proto, APP_APPMGR);
}

void WorldProxyGather::HandleCmdWorldProxy(Packet* pPacket)
{
    LOG_DEBUG("------------------------------------");
    LOG_DEBUG("**** world proxy gather ****");

    const auto pResMgr = ResourceHelp::GetResourceManager();
    for (auto &one : _maps)
    {
        const auto pWorldRes = pResMgr->Worlds->GetResource(one.second.WorldId);
        LOG_DEBUG("sn:" << one.first
            << " proxy sn:" << one.second.WorldSn
            << " online:" << one.second.Online
            << " online:" << one.second.Online
            << " name:" << pWorldRes->GetName().c_str());
    }
}

void WorldProxyGather::HandleWorldProxySyncToGather(Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Proto::WorldProxySyncToGather>();
    const uint64 worldSn = proto.world_sn();
    const int worldId = proto.world_id();
    const auto isRemove = proto.is_remove();

    if (!isRemove)
    {
        const int online = proto.online();

        auto iter = _maps.find(worldSn);
        if (iter == _maps.end())
        {
            _maps[worldSn] = WorldProxyInfo();
            _maps[worldSn].WorldId = worldId;
            _maps[worldSn].WorldSn = worldSn;
        }

        _maps[worldSn].Online = online;
    }
    else
    {
        _maps.erase(worldSn);
    }
}

void WorldProxyGather::HandleNetworkDisconnect(Packet* pPacket)
{
    auto pTagValue = pPacket->GetTagKey()->GetTagValue(TagType::Account);
    const auto pPlayer = playerMgr->GetPlayerBySocket(pPacket->GetSocketKey()->Socket);
    if (pTagValue == nullptr || pPlayer == nullptr)
        return;
    if (pPlayer->pTeam)
    {
        Proto::CreateTeam proto;
        Team* pTeam = teamMap[pPlayer->GetPlayerSN()];
        pTeam->RemoveMember(pPlayer->GetPlayerSN());
        proto.set_captain(pTeam->GetCaptain());
        for (uint64 sn : pTeam->GetMembers())
            proto.add_members(sn);
        for (uint64 sn : pTeam->GetMembers())
        {
            Player* tmp = playerMgr->GetPlayerBySn(sn);
            MessageSystemHelp::SendPacket(Proto::MsgId::MI_CreateTeam, proto, tmp);
        }
    }
    GetComponent<PlayerCollectorComponent>()->RemovePlayerBySocket(pPacket->GetSocketKey()->Socket);
}

void WorldProxyGather::HandleLoginByToken(Packet* pPacket)
{
    auto pPlayerCollector = GetComponent<PlayerCollectorComponent>();

    auto proto = pPacket->ParseToProto<Proto::LoginByToken>();
    auto pPlayer = pPlayerCollector->AddPlayer(pPacket, proto.account());
    if (pPlayer == nullptr)
    {
        MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_NetworkRequestDisconnect, pPacket);
        return;
    }
}

void WorldProxyGather::HandleQueryPlayerRs(Packet* pPacket)
{
    //从数据库中读取到player数据
    auto& protoRs = pPacket->ParseToProto<Proto::QueryPlayerRs>();
    auto& account = protoRs.account();
    auto pPlayer = GetComponent<PlayerCollectorComponent>()->GetPlayerByAccount(account);
    if (pPlayer == nullptr)
    {
        LOG_ERROR("HandleQueryPlayer. pPlayer == nullptr. account:" << account.c_str());
        return;
    }
    auto& protoPlayer = protoRs.player();
    const auto& playerSn = protoPlayer.sn();
    pPlayer->ParserFromProto(playerSn, protoPlayer);
}

void WorldProxyGather::HandleCreateTeam(Packet* pPacket)
{
    Proto::CreateTeam proto = pPacket->ParseToProto<Proto::CreateTeam>();
    Team* pTeam = new Team(proto.captain());
    for (uint64 sn : proto.members())
    {
        pTeam->AddMember(sn);
        teamMap.emplace(std::make_pair(sn, pTeam));
        Player* tmp = playerMgr->GetPlayerBySn(sn);
        tmp->pTeam = pTeam;
        MessageSystemHelp::SendPacket(Proto::MsgId::MI_CreateTeam, proto, tmp);
    }
}
