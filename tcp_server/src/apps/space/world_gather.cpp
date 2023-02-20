#include "world_gather.h"
#include "libserver/thread_mgr.h"
#include "libserver/network_locator.h"
#include "libserver/message_system_help.h"
#include "libserver/global.h"
#include "libserver/message_system.h"

void WorldGather::Awake()
{
	playerMgr = AddComponent<PlayerManagerComponent>();
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
}

void WorldGather::AddWorld(uint64 sn, World* world)
{
	_worlds.emplace(sn, world);
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

void WorldGather::HandleNetworkDisconnect(Packet* pPacket)
{
	auto pTags = pPacket->GetTagKey();
	const auto pTagPlayer = pTags->GetTagValue(TagType::Player);
	if (pTagPlayer != nullptr)
	{
		const auto pPlayer = playerMgr->GetPlayerBySn(pTagPlayer->KeyInt64);
		//if (pPlayer && pPlayer->pTeam)
		//{
		//	Proto::CreateTeam proto;
		//	Team* pTeam = teamMap[pPlayer->GetPlayerSN()];
		//	pTeam->RemoveMember(pPlayer->GetPlayerSN());
		//	proto.set_captain(pTeam->GetCaptain());
		//	for (uint64 sn : pTeam->GetMembers())
		//		proto.add_members(sn);
		//	for (uint64 sn : pTeam->GetMembers())
		//	{
		//		Player* tmp = playerMgr->GetPlayerBySn(sn);
		//		MessageSystemHelp::SendPacket(Proto::MsgId::MI_CreateTeam, proto, tmp);
		//	}
		//}
		playerMgr->RemovePlayerBySn(pTagPlayer->KeyInt64);
	}
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
	//Team* pTeam = new Team(proto.captain());
	//for (uint64 sn : proto.members())
	//{
	//	pTeam->AddMember(sn);
	//	teamMap.emplace(std::make_pair(sn, pTeam));
	//	Player* tmp = playerMgr->GetPlayerBySn(sn);
	//	tmp->pTeam = pTeam;
	//	MessageSystemHelp::SendPacket(Proto::MsgId::MI_CreateTeam, proto, tmp);
	//}
}
