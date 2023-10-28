#include "world_gather.h"
#include "libserver/thread_mgr.h"
#include "libserver/network_locator.h"
#include "libserver/message_system_help.h"
#include "libserver/global.h"
#include "libserver/message_system.h"

void WorldGather::Awake()
{
	AddTimer(0, 10, true, 2, BindFunP0(this, &WorldGather::SyncSpaceInfo));

    auto pMsgSystem = GetSystemManager()->GetMessageSystem();
	pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_CmdWorld, BindFunP1(this, &WorldGather::HandleCmdWorld));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_WorldSyncToGather, BindFunP1(this, &WorldGather::HandleWorldSyncToGather));
	pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_DungeonDisapper, BindFunP1(this, &WorldGather::HandleDungeonDisapper));
	pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_CreateTeam, BindFunP1(this, &WorldGather::HandleCreateTeam));
}

void WorldGather::BackToPool()
{
	_worldOnlines.clear();
	_players.clear();
	teamMap.clear();
}

void WorldGather::RegistPlayer(uint64 sn, Player* player)
{
	if (player)
		_players[sn] = player;
	else
	{
		_players.erase(sn);
		if (teamMap.find(sn) != teamMap.end())
			teamMap[sn]->RemoveMember(sn);
	}
}

Player* WorldGather::GetPlayerBySn(uint64 sn)
{
	if (_players.find(sn) == _players.end())
		return nullptr;
	return _players[sn];
}

void WorldGather::SyncSpaceInfo()
{
	Proto::AppInfoSync proto;
	int online = 0;
	auto iter = _worldOnlines.begin();
	while (iter != _worldOnlines.end())
	{
		online += iter->second;
		++iter;
	}

	const auto pGlobal = Global::GetInstance();
	proto.set_app_id(pGlobal->GetCurAppId());
	proto.set_app_type((int)pGlobal->GetCurAppType());
	proto.set_online(online);

    // 对链接的所有Game发送本进程当前状态
    MessageSystemHelp::SendPacketToAllApp(Proto::MsgId::MI_AppInfoSync, proto, APP_GAME);

    // 发送给appmgr
    MessageSystemHelp::SendPacket(Proto::MsgId::MI_AppInfoSync, proto, APP_APPMGR);
}

void WorldGather::HandleCmdWorld(Packet* pPacket)
{
	LOG_DEBUG("**** world gather ****");
	for (auto& one : _worldOnlines)
		LOG_DEBUG("sn:" << one.first << " online:" << one.second);
}

void WorldGather::HandleWorldSyncToGather(Packet* pPacket)
{
	auto proto = pPacket->ParseToProto<Proto::WorldSyncToGather>();
	_worldOnlines[proto.world_sn()] = proto.online();
}

void WorldGather::HandleDungeonDisapper(Packet* pPacket)
{
	auto proto = pPacket->ParseToProto<Proto::DungeonDisapper>();
	_worldOnlines.erase(proto.world_sn());
}

void WorldGather::HandleCreateTeam(Packet* pPacket)
{
	Proto::CreateTeam proto = pPacket->ParseToProto<Proto::CreateTeam>();
	SpaceTeam* pTeam = new SpaceTeam(proto.captain());
	for (const uint64& sn : proto.members())
	{
		pTeam->AddMember(sn);
		teamMap.emplace(sn, pTeam);
	}
}
