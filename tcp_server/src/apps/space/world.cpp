﻿#include "world.h"

void World::Awake(int worldId)
{
	//LOG_DEBUG("create world. id:" << worldId << " sn:" << _sn << " space app id:" << Global::GetAppIdFromSN(_sn));
	_worldId = worldId;
	_playerManager = AddComponent<PlayerManagerComponent>();
	AddTimer(0, 10, false, 0, BindFunP0(this, &World::SyncWorldToGather));
	AddTimer(0, 1, false, 0, BindFunP0(this, &World::SyncAppearTimer));
	AddTimer(0, 1, false, 0, BindFunP0(this, &World::SyncEnemiesTimer));

	// message
	auto pMsgSystem = GetSystemManager()->GetMessageSystem();
	pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_NetworkDisconnect, BindFunP1(this, &World::HandleNetworkDisconnect));
	pMsgSystem->RegisterFunction(this, Proto::MsgId::G2S_SyncPlayer, BindFunP1(this, &World::HandleSyncPlayer));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::G2S_RequestSyncPlayer, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleRequestSyncPlayer));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::G2S_RemovePlayer, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleG2SRemovePlayer));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2S_Move, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleMove));

	ResourceWorld* worldCfg = ResourceHelp::GetResourceManager()->Worlds->GetResource(_worldId);
	std::vector<ResourceEnemy> _enemyCfgs = worldCfg->GetEnemies();
	for (auto cfg : _enemyCfgs)
	{
		AIEnemy* enemy = GetSystemManager()->GetEntitySystem()->AddComponent<AIEnemy>(cfg.id, cfg.initHp, cfg.initPos);
		enemy->SetWorld(this);
		enemy->AddComponent<AIComponent>();
		_enemies.push_back(enemy);
	}
}

void World::BackToPool()
{
	_addPlayer.clear();
}

/// <summary>
/// 通过player sn找到地图中指定玩家
/// game线程和space线程通信时打标签
/// </summary>
/// <param name="pIdentify"></param>
/// <returns></returns>
Player* World::GetPlayer(NetIdentify* pIdentify)
{
	auto pTags = pIdentify->GetTagKey();
	const auto pTagPlayer = pTags->GetTagValue(TagType::Player);
	if (pTagPlayer == nullptr)
		return nullptr;
	auto pPlayerMgr = GetComponent<PlayerManagerComponent>();
	return pPlayerMgr->GetPlayerBySn(pTagPlayer->KeyInt64);
}

void World::HandleNetworkDisconnect(Packet* pPacket)
{
	//LOG_DEBUG("world id:" << _worldId << " disconnect." << pPacket);

	auto pTags = pPacket->GetTagKey();
	const auto pTagPlayer = pTags->GetTagValue(TagType::Player);
	if (pTagPlayer != nullptr)
	{
		auto pPlayerMgr = GetComponent<PlayerManagerComponent>();
		const auto pPlayer = pPlayerMgr->GetPlayerBySn(pTagPlayer->KeyInt64);
		if (pPlayer == nullptr)
		{
			LOG_ERROR("world. net disconnect. can't find player. player sn:" << pTagPlayer->KeyInt64);
			return;
		}

		Proto::SavePlayer protoSave;
		protoSave.set_player_sn(pPlayer->GetPlayerSN());
		pPlayer->SerializeToProto(protoSave.mutable_player());
		MessageSystemHelp::SendPacket(Proto::MsgId::G2DB_SavePlayer, protoSave, APP_DB_MGR);

		// 玩家掉线
		pPlayerMgr->RemovePlayerBySn(pTagPlayer->KeyInt64);
	}
	else
	{
		// dbmgr, appmgr or game断线
		const auto pTagApp = pTags->GetTagValue(TagType::App);
		if (pTagApp != nullptr)
		{
			auto pPlayerMgr = GetComponent<PlayerManagerComponent>();
			pPlayerMgr->RemoveAllPlayers(pPacket);
		}
	}
}

void World::SyncWorldToGather()
{
	Proto::WorldSyncToGather proto;
	proto.set_world_sn(GetSN());
	proto.set_world_id(GetWorldId());

	const int online = GetComponent<PlayerManagerComponent>()->OnlineSize();
	proto.set_online(online);

	MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_WorldSyncToGather, proto, nullptr);
}

inline void CreateProtoRoleAppear(Player* pPlayer, Proto::RoleAppear& protoAppear)
{
	auto proto = protoAppear.add_role();
	proto->set_name(pPlayer->GetName().c_str());
	proto->set_sn(pPlayer->GetPlayerSN());

	const auto pBaseInfo = pPlayer->GetComponent<PlayerComponentDetail>();
	proto->set_gender(pBaseInfo->GetGender());

	const auto pComponentLastMap = pPlayer->GetComponent<PlayerComponentLastMap>();
	const auto pLastMap = pComponentLastMap->GetCur();
	pLastMap->Position.SerializeToProto(proto->mutable_position());
}

void World::SyncAppearTimer()
{
	auto pPlayerMgr = GetComponent<PlayerManagerComponent>();

	if (!_addPlayer.empty())
	{
		// 1.新增的数据，同步到全地图
		Proto::RoleAppear protoNewAppear;
		for (auto id : _addPlayer)
		{
			// 有可能瞬间已下线
			const auto pPlayer = pPlayerMgr->GetPlayerBySn(id);
			if (pPlayer == nullptr)
				continue;

			CreateProtoRoleAppear(pPlayer, protoNewAppear);
		}

		if (protoNewAppear.role_size() > 0)
			BroadcastPacket(Proto::MsgId::S2C_RoleAppear, protoNewAppear);

		// 2.原始玩家的数据，同步给新增的玩家
		Proto::RoleAppear protoOther;
		const auto players = pPlayerMgr->GetAll();
		for (const auto one : *players)
		{
			// 排除新玩家
			if (_addPlayer.find(one.first) != _addPlayer.end())
				continue;

			const auto role = one.second;
			CreateProtoRoleAppear(role, protoOther);
		}

		if (protoOther.role_size() > 0)
			BroadcastPacket(Proto::MsgId::S2C_RoleAppear, protoOther, _addPlayer);

		_addPlayer.clear();
	}
}

void World::SyncEnemiesTimer()
{
	if (!_enemies.empty())
	{
		Proto::EnemyList protoList;
		for (auto enemy : _enemies)
		{
			Proto::Enemy* protoEnemy = protoList.add_enemies();
			protoEnemy->set_id(enemy->GetID());
			enemy->GetPos().SerializeToProto(protoEnemy->mutable_pos());
		}
		BroadcastPacket(Proto::MsgId::S2C_EnemyList, protoList);
	}
}

/// <summary>
/// 将player添加到world实例中, 通知客户端加载地图
/// </summary>
/// <param name="pPacket"></param>
void World::HandleSyncPlayer(Packet* pPacket)
{
	auto proto = pPacket->ParseToProto<Proto::SyncPlayer>();
	const auto playerSn = proto.player().sn();
	//const int gameAppId = proto.app_id();

	auto pPlayerMgr = GetComponent<PlayerManagerComponent>();
	auto pPlayer = pPlayerMgr->AddPlayer(playerSn, GetSN(), pPacket);
	if (pPlayer == nullptr)
	{
		LOG_ERROR("failed to add player. player sn:" << playerSn);
		return;
	}

	pPlayer->ParserFromProto(playerSn, proto.player());
	pPlayer->AddComponent<PlayerComponentDetail>();

	const auto pComponentLastMap = pPlayer->AddComponent<PlayerComponentLastMap>();
	pComponentLastMap->EnterWorld(_worldId, _sn);
	const auto pLastMap = pComponentLastMap->GetCur();

	//LOG_DEBUG("world. recv teleport. map id:" << _worldId << " world sn:" << GetSN() << " playerSn:" << pPlayer->GetPlayerSN());

	//通知客户端进入地图
	Proto::EnterWorld protoEnterWorld;
	protoEnterWorld.set_world_id(_worldId);
	pLastMap->Position.SerializeToProto(protoEnterWorld.mutable_position());
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_EnterWorld, protoEnterWorld, pPlayer);
	_addPlayer.insert(playerSn);
}

void World::BroadcastPacket(Proto::MsgId msgId, google::protobuf::Message& proto)
{
	auto pPlayerMgr = GetComponent<PlayerManagerComponent>();
	const auto players = pPlayerMgr->GetAll();
	for (const auto pair : *players)
	{
		//LOG_DEBUG("broadcast msgId:" << Log4Help::GetMsgIdName(msgId).c_str() << " player sn:" << pair.second->GetPlayerSN());
		MessageSystemHelp::SendPacket(msgId, proto, pair.second);
	}
}

void World::BroadcastPacket(Proto::MsgId msgId, google::protobuf::Message& proto, std::set<uint64> players)
{
	auto pPlayerMgr = GetComponent<PlayerManagerComponent>();
	for (const auto one : players)
	{
		const auto pPlayer = pPlayerMgr->GetPlayerBySn(one);
		if (pPlayer == nullptr)
			continue;

		//LOG_DEBUG("broadcast msgId:" << Log4Help::GetMsgIdName(msgId).c_str() << " player sn:" << one);
		MessageSystemHelp::SendPacket(msgId, proto, pPlayer);
	}
}

void World::HandleRequestSyncPlayer(Player* pPlayer, Packet* pPacket)
{
	Proto::SyncPlayer protoSync;
	protoSync.set_account(pPlayer->GetAccount().c_str());
	pPlayer->SerializeToProto(protoSync.mutable_player());

	MessageSystemHelp::SendPacket(Proto::MsgId::S2G_SyncPlayer, protoSync, pPlayer);
}

void World::HandleG2SRemovePlayer(Player* pPlayer, Packet* pPacket)
{
	auto proto = pPacket->ParseToProto<Proto::RemovePlayer>();
	if (proto.player_sn() != pPlayer->GetPlayerSN())
	{
		LOG_ERROR("HandleTeleportAfter. proto.player_sn() != pPlayer->GetPlayerSN()");
		return;
	}

	auto pPlayerMgr = GetComponent<PlayerManagerComponent>();
	pPlayerMgr->RemovePlayerBySn(pPlayer->GetPlayerSN());

	// 通知其他玩家
	Proto::RoleDisAppear disAppear;
	disAppear.set_sn(pPlayer->GetPlayerSN());
	BroadcastPacket(Proto::MsgId::S2C_RoleDisAppear, disAppear);
	LOG_DEBUG("player disappear: sn=" << pPlayer->GetPlayerSN());
}

void World::HandleMove(Player* pPlayer, Packet* pPacket)
{
	auto proto = pPacket->ParseToProto<Proto::Move>();
	const auto positions = proto.mutable_position();
	int enemyId = proto.enemy_id();

	if (enemyId == -1)
	{
		proto.set_player_sn(pPlayer->GetPlayerSN());
		auto pMoveComponent = pPlayer->GetComponent<MoveComponent>();
		if (pMoveComponent == nullptr)
			pMoveComponent = pPlayer->AddComponent<MoveComponent>();

		std::queue<Vector3> pos;
		for (auto index = 0; index < proto.position_size(); index++)
		{
			Vector3 v3(0, 0, 0);
			v3.ParserFromProto(positions->Get(index));
			pos.push(v3);
		}
		const auto pComponentLastMap = pPlayer->GetComponent<PlayerComponentLastMap>();
		pMoveComponent->Update(pos, pComponentLastMap->GetCur()->Position);
	}
	else
	{
		if (proto.position_size() > 0)
		{
			Vector3 pos;
			pos.ParserFromProto(positions->Get(proto.position_size() - 1));
			_enemies[enemyId]->SetPos(pos);
		}
	}

	BroadcastPacket(Proto::MsgId::S2C_Move, proto);
}
