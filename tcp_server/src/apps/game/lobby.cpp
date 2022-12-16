#include "lobby.h"

#include "libserver/message_system_help.h"

#include "libplayer/player_collector_component.h"
#include "libplayer/player_component_proto_list.h"
#include "libserver/message_system.h"

#include "world_proxy_component_gather.h"
#include "player_component_onlinegame.h"
#include "player_component_token.h"
#include "libplayer/player.h"
#include "libplayer/player_component_last_map.h"
#include "world_proxy_help.h"
#include "world_proxy_locator.h"
#include "libresource/resource_help.h"
#include "libserver/socket_locator.h"

void Lobby::Awake()
{
	auto pResMgr = ResourceHelp::GetResourceManager();
	_worldId = pResMgr->Worlds->GetRolesMap()->GetId();

	AddComponent<PlayerCollectorComponent>();
	AddComponent<WorldProxyComponentGather>();

	// locator
	auto pProxyLocator = ComponentHelp::GetGlobalEntitySystem()->GetComponent<WorldProxyLocator>();
	pProxyLocator->RegisterToLocator(_worldId, GetSN());

	// message
	auto pMsgSystem = GetSystemManager()->GetMessageSystem();

	pMsgSystem->RegisterFunction(this, Net::MsgId::MI_NetworkDisconnect, BindFunP1(this, &Lobby::HandleNetworkDisconnect));
	pMsgSystem->RegisterFunction(this, Net::MsgId::C2G_LoginByToken, BindFunP1(this, &Lobby::HandleLoginByToken));
	pMsgSystem->RegisterFunction(this, Net::MsgId::MI_GameTokenToRedisRs, BindFunP1(this, &Lobby::HandleGameTokenToRedisRs));
	pMsgSystem->RegisterFunction(this, Net::MsgId::G2DB_QueryPlayerRs, BindFunP1(this, &Lobby::HandleQueryPlayerRs));
	pMsgSystem->RegisterFunction(this, Net::MsgId::G2M_QueryWorldRs, BindFunP1(this, &Lobby::HandleQueryWorldRs));

	pMsgSystem->RegisterFunction(this, Net::MsgId::MI_BroadcastCreateWorldProxy, BindFunP1(this, &Lobby::HandleBroadcastCreateWorldProxy));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Net::MsgId::MI_TeleportAfter, BindFunP1(this, &Lobby::GetPlayer), BindFunP2(this, &Lobby::HandleTeleportAfter));
}

void Lobby::BackToPool()
{
	_waitingForWorld.clear();
}

Player* Lobby::GetPlayer(NetIdentify* pIdentify)
{
	auto pTagValue = pIdentify->GetTagKey()->GetTagValue(TagType::Player);
	if (pTagValue == nullptr)
		return nullptr;

	const auto playerSn = pTagValue->KeyInt64;

	auto pPlayerCollector = GetComponent<PlayerCollectorComponent>();
	return pPlayerCollector->GetPlayerBySn(playerSn);
}

void Lobby::HandleNetworkDisconnect(Packet* pPacket)
{
	auto pTagValue = pPacket->GetTagKey()->GetTagValue(TagType::Account);
	if (pTagValue == nullptr)
		return;

	GetComponent<PlayerCollectorComponent>()->RemovePlayerBySocket(pPacket->GetSocketKey()->Socket);
}

void Lobby::HandleLoginByToken(Packet* pPacket)
{
	auto pPlayerCollector = GetComponent<PlayerCollectorComponent>();

	auto proto = pPacket->ParseToProto<Net::LoginByToken>();
	auto pPlayer = pPlayerCollector->AddPlayer(pPacket, proto.account());
	if (pPlayer == nullptr)
	{
		MessageSystemHelp::DispatchPacket(Net::MsgId::MI_NetworkRequestDisconnect, pPacket);
		return;
	}

	pPlayer->AddComponent<PlayerComponentToken>(proto.token());
	pPlayer->AddComponent<PlayerComponentOnlineInGame>(pPlayer->GetAccount(), 1);

	Net::GameTokenToRedis protoToken;
	protoToken.set_account(pPlayer->GetAccount().c_str());
	MessageSystemHelp::DispatchPacket(Net::MsgId::MI_GameTokenToRedis, protoToken, nullptr);

	// 注册SOCKET
	auto pSocketLocator = ComponentHelp::GetGlobalEntitySystem()->GetComponent<SocketLocator>();
	pSocketLocator->RegisterToLocator(pPlayer->GetSocketKey()->Socket, GetSN());
}

void Lobby::HandleGameTokenToRedisRs(Packet* pPacket)
{
	auto protoRs = pPacket->ParseToProto<Net::GameTokenToRedisRs>();
	auto pPlayer = GetComponent<PlayerCollectorComponent>()->GetPlayerByAccount(protoRs.account());
	if (pPlayer == nullptr)
	{
		LOG_ERROR("HandleGameRequestTokenToRedisRs. pPlayer == nullptr. account:" << protoRs.account().c_str());
		return;
	}

	Net::LoginByTokenRs protoLoginGameRs;
	protoLoginGameRs.set_return_code(Net::LoginByTokenRs::LGRC_TOKEN_WRONG);
	const auto pTokenComponent = pPlayer->GetComponent<PlayerComponentToken>();
	if (pTokenComponent->IsTokenValid(protoRs.token_info().token()))
	{
		protoLoginGameRs.set_return_code(Net::LoginByTokenRs::LGRC_OK);
	}

	MessageSystemHelp::SendPacket(Net::MsgId::C2G_LoginByTokenRs, protoLoginGameRs, pPlayer);

	if (protoLoginGameRs.return_code() != Net::LoginByTokenRs::LGRC_OK)
		return;

	//LOG_DEBUG("enter game. account:" << pPlayer->GetAccount().c_str() << " token:" << protoRs.token_info().token().c_str());

	// 查询玩家数据	
	Net::QueryPlayer protoQuery;
	protoQuery.set_player_sn(protoRs.token_info().player_sn());
	MessageSystemHelp::SendPacket(Net::MsgId::G2DB_QueryPlayer, protoQuery, APP_DB_MGR);
}

void Lobby::HandleQueryPlayerRs(Packet* pPacket)
{
	//从数据库中读取到player数据
	auto protoRs = pPacket->ParseToProto<Net::QueryPlayerRs>();
	auto account = protoRs.account();
	auto pPlayer = GetComponent<PlayerCollectorComponent>()->GetPlayerByAccount(account);
	if (pPlayer == nullptr)
	{
		LOG_ERROR("HandleQueryPlayer. pPlayer == nullptr. account:" << account.c_str());
		return;
	}

	// 向客户端发送玩家数据
	Net::SyncPlayer syncPlayer;
	syncPlayer.mutable_player()->CopyFrom(protoRs.player());
	MessageSystemHelp::SendPacket(Net::MsgId::G2C_SyncPlayer, syncPlayer, pPlayer);

	// 分析进入地图
	auto protoPlayer = protoRs.player();
	const auto playerSn = protoPlayer.sn();
	pPlayer->ParserFromProto(playerSn, protoPlayer);
	const auto pPlayerLastMap = pPlayer->AddComponent<PlayerComponentLastMap>();
	auto pWorldLocator = ComponentHelp::GetGlobalEntitySystem()->GetComponent<WorldProxyLocator>();

	// 进入副本
	auto pLastMap = pPlayerLastMap->GetLastDungeon();
	if (pLastMap != nullptr)
	{
		if (pWorldLocator->IsExistDungeon(pLastMap->WorldSn))
		{
			// 地图代理中存在副本，跳转
			WorldProxyHelp::Teleport(pPlayer, GetSN(), pLastMap->WorldSn);
			return;
		}

		// 查询副本是否存在
		if (_waitingForDungeon.find(pLastMap->WorldSn) == _waitingForDungeon.end())
		{
			_waitingForDungeon[pLastMap->WorldSn] = std::set<uint64>();
		}

		if (_waitingForDungeon[pLastMap->WorldSn].empty())
		{
			// 向appmgr查询副本(单线程)
			Net::QueryWorld protoToMgr;
			protoToMgr.set_world_sn(pLastMap->WorldSn);
			protoToMgr.set_last_world_sn(GetSN());
			MessageSystemHelp::SendPacket(Net::MsgId::G2M_QueryWorld, protoToMgr, APP_APPMGR);
		}

		_waitingForDungeon[pLastMap->WorldSn].insert(pPlayer->GetPlayerSN());
		return;

	}

	EnterPublicWorld(pPlayer);
}

void Lobby::EnterPublicWorld(Player* pPlayer)
{
	const auto pPlayerLastMap = pPlayer->GetComponent<PlayerComponentLastMap>();
	auto pWorldLocator = ComponentHelp::GetGlobalEntitySystem()->GetComponent<WorldProxyLocator>();

	// 进入公共地图
	auto pLastMap = pPlayerLastMap->GetLastPublicMap();
	const auto lastMapSn = pWorldLocator->GetWorldSnById(pLastMap->WorldId);
	if (lastMapSn != (uint64)INVALID_ID)
	{
		//LOG_DEBUG("teleport to public. world id:" << pLastMap->WorldId);

		// 存在公共地图，跳转
		WorldProxyHelp::Teleport(pPlayer, GetSN(), lastMapSn);
		return;
	}

	// 等待跳转
	if (_waitingForWorld.find(pLastMap->WorldId) == _waitingForWorld.end())
	{
		_waitingForWorld[pLastMap->WorldId] = std::set<uint64>();
	}

	if (_waitingForWorld[pLastMap->WorldId].empty())
	{
		// 向appmgr申请创建地图(单线程)
		Net::RequestWorld protoToMgr;
		protoToMgr.set_world_id(pLastMap->WorldId);
		MessageSystemHelp::SendPacket(Net::MsgId::G2M_RequestWorld, protoToMgr, APP_APPMGR);
	}

	_waitingForWorld[pLastMap->WorldId].insert(pPlayer->GetPlayerSN());
}

void Lobby::HandleQueryWorldRs(Packet* pPacket)
{
	auto proto = pPacket->ParseToProto<Net::QueryWorldRs>();
	const auto worldSn = proto.world_sn();

	const auto iter = _waitingForDungeon.find(worldSn);
	if (iter == _waitingForDungeon.end())
		return;

	if (proto.return_code() == Net::QueryWorldRs::QueryWorld_OK)
		return;

	auto pPlayerMgr = GetComponent<PlayerCollectorComponent>();

	auto players = iter->second;
	for (auto one : players)
	{
		const auto pPlayer = pPlayerMgr->GetPlayerBySn(one);
		if (pPlayer == nullptr)
			continue;

		// 查询副本失败，进入公共地图
		EnterPublicWorld(pPlayer);
	}

	_waitingForDungeon.erase(iter);
}

/// <summary>
/// 收到地图创建成功的消息后, 跳转地图
/// </summary>
/// <param name="pPacket"></param>
void Lobby::HandleBroadcastCreateWorldProxy(Packet* pPacket)
{
	auto proto = pPacket->ParseToProto<Net::BroadcastCreateWorldProxy>();
	const auto worldId = proto.world_id();
	const auto worldSn = proto.world_sn();

	auto pResMgr = ResourceHelp::GetResourceManager();
	auto pWorldRes = pResMgr->Worlds->GetResource(worldId);
	if (pWorldRes == nullptr)
	{
		LOG_ERROR("can't find resouces of world. world id:" << worldId);
		return;
	}

	if (pWorldRes->IsType(ResourceWorldType::Public))
	{
		const auto iter = _waitingForWorld.find(worldId);
		if (iter == _waitingForWorld.end())
			return;

		//LOG_DEBUG("recv worldproxy create msg. map id:" << worldId << " world sn:" << worldSn);

		auto pPlayerMgr = GetComponent<PlayerCollectorComponent>();

		// 这里一定是公共地图
		auto players = iter->second;
		for (auto one : players)
		{
			const auto player = pPlayerMgr->GetPlayerBySn(one);
			if (player == nullptr)
				continue;

			WorldProxyHelp::Teleport(player, GetSN(), worldSn);
		}

		_waitingForWorld.erase(iter);
	}
	else
	{
		// 副本一定是定向发送，如果找不到，肯定有BUG
		auto iter = _waitingForDungeon.find(worldSn);
		if (iter == _waitingForDungeon.end())
		{
			LOG_ERROR("can't find player. world id:" << worldId);
			return;
		}

		auto pPlayerMgr = GetComponent<PlayerCollectorComponent>();
		auto players = iter->second;
		for (auto one : players)
		{
			const auto player = pPlayerMgr->GetPlayerBySn(one);
			if (player == nullptr)
				continue;

			WorldProxyHelp::Teleport(player, GetSN(), worldSn);
		}

		_waitingForDungeon.erase(iter);
	}
}

void Lobby::HandleTeleportAfter(Player* pPlayer, Packet* pPacket)
{
	//LOG_DEBUG("teleport after. remove account:" << pPlayer->GetAccount().c_str());
	auto pPlayerMgr = GetComponent<PlayerCollectorComponent>();
	pPlayerMgr->RemovePlayerBySocket(pPlayer->GetSocketKey()->Socket);
}

