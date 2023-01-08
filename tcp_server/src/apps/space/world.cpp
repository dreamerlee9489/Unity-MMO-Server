#include "world.h"
#include "fsm_state.h"

void World::Awake(int worldId)
{
	//LOG_DEBUG("create world. id:" << worldId << " sn:" << _sn << " space app id:" << Global::GetAppIdFromSN(_sn));
	_worldId = worldId;
	playerMgr = AddComponent<PlayerManagerComponent>();
	AddTimer(0, 10, false, 0, BindFunP0(this, &World::SyncWorldToGather));
	AddTimer(0, 1, false, 0, BindFunP0(this, &World::SyncAppearTimer));

	// message
	auto pMsgSystem = GetSystemManager()->GetMessageSystem();
	pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_NetworkDisconnect, BindFunP1(this, &World::HandleNetworkDisconnect));
	pMsgSystem->RegisterFunction(this, Proto::MsgId::G2S_SyncPlayer, BindFunP1(this, &World::HandleSyncPlayer));
	pMsgSystem->RegisterFunction(this, Proto::MsgId::C2S_SyncNpcPos, BindFunP1(this, &World::HandleSyncNpcPos));
	pMsgSystem->RegisterFunction(this, Proto::MsgId::C2S_PlayerAtkEvent, BindFunP1(this, &World::HandlePlayerAtkEvent));
	pMsgSystem->RegisterFunction(this, Proto::MsgId::C2S_NpcAtkEvent, BindFunP1(this, &World::HandleNpcAtkEvent));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::G2S_RequestSyncPlayer, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleRequestSyncPlayer));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::G2S_RemovePlayer, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleG2SRemovePlayer));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2S_Move, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleMove));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2S_SyncPlayerPos, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleSyncPlayerPos));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2S_SyncPlayerCmd, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleSyncPlayerCmd));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2S_ReqSyncNpc, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleReqSyncNpc));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2S_UpdateKnapItem, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleUpdateKnapItem));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2S_GetPlayerKnap, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleGetPlayerKnap));

	ResourceWorld* worldCfg = ResourceHelp::GetResourceManager()->Worlds->GetResource(_worldId);
	std::vector<ResourceNpc>& npcCfgs = *worldCfg->GetNpcCfgs();
	potionCfgs = worldCfg->GetPotionCfgs();
	weaponCfgs = worldCfg->GetWeaponCfgs();
	for (auto& cfg : npcCfgs)
	{
		Npc* npc = GetSystemManager()->GetEntitySystem()->AddComponent<Npc>(cfg);
		npc->SetWorld(this);
		npc->SetAllPlayer(playerMgr->GetAll());
		npc->AddComponent<FsmComponent>();
		npcIdxMap.emplace(npc->GetSN(), npcs.size());
		npcs.push_back(npc);
	}
}

void World::BackToPool()
{
	_adds.clear();
	npcs.clear();
	npcIdxMap.clear();
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
		Proto::RoleDisappear protoDis;
		protoDis.set_sn(pPlayer->GetPlayerSN());
		BroadcastPacket(Proto::MsgId::S2C_RoleDisappear, protoDis);
		pPlayerMgr->RemovePlayerBySn(pTagPlayer->KeyInt64);

		for (auto& enemy : npcs)
		{
			if (enemy->GetLinkPlayer() == pPlayer)
			{
				enemy->GetComponent<FsmComponent>()->ResetState();
				Player* target = GetNearestPlayer(enemy->GetCurrPos());
				enemy->SetLinkPlayer(target);
				if (target)
				{
					Proto::ReqLinkPlayer proto;
					proto.set_npc_id(enemy->GetID());
					proto.set_npc_sn(enemy->GetSN());
					proto.set_linker(true);
					MessageSystemHelp::SendPacket(Proto::MsgId::S2C_ReqLinkPlayer, proto, target);
				}
			}
		}
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

inline void AddToAllRoleAppear(Player* pPlayer, Proto::AllRoleAppear& protoAppear)
{
	Proto::Role* proto = protoAppear.add_roles();
	proto->set_name(pPlayer->GetName().c_str());
	proto->set_sn(pPlayer->GetPlayerSN());
	proto->set_level(pPlayer->detail->lv);
	proto->set_xp(pPlayer->detail->xp);
	proto->set_hp(pPlayer->detail->hp);
	proto->set_mp(pPlayer->detail->mp);
	proto->set_atk(pPlayer->detail->atk);
	proto->set_def(pPlayer->detail->def);

	const auto pBaseInfo = pPlayer->GetComponent<PlayerComponentDetail>();
	proto->set_gender(pBaseInfo->GetGender());

	const auto pComponentLastMap = pPlayer->GetComponent<PlayerComponentLastMap>();
	const auto pLastMap = pComponentLastMap->GetCur();
	pLastMap->Position.SerializeToProto(proto->mutable_position());
}

void World::SyncAppearTimer()
{
	auto pPlayerMgr = GetComponent<PlayerManagerComponent>();

	if (!_adds.empty())
	{
		// 1.新增的数据，同步到全地图
		Proto::AllRoleAppear protoNewAppear;
		for (auto& id : _adds)
		{
			// 有可能瞬间已下线
			const auto pPlayer = pPlayerMgr->GetPlayerBySn(id);
			if (pPlayer == nullptr)
				continue;

			AddToAllRoleAppear(pPlayer, protoNewAppear);
		}

		if (protoNewAppear.roles_size() > 0)
			BroadcastPacket(Proto::MsgId::S2C_AllRoleAppear, protoNewAppear);

		// 2.原始玩家的数据，同步给新增的玩家
		Proto::AllRoleAppear protoOther;
		const auto players = pPlayerMgr->GetAll();
		for (const auto& one : *players)
		{
			// 排除新玩家
			if (_adds.find(one.first) != _adds.end())
				continue;

			const auto role = one.second;
			AddToAllRoleAppear(role, protoOther);
		}

		if (protoOther.roles_size() > 0)
			BroadcastPacket(Proto::MsgId::S2C_AllRoleAppear, protoOther, _adds);

		_adds.clear();
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
	pPlayer->detail = pPlayer->AddComponent<PlayerComponentDetail>();
	pPlayer->lastMap = pPlayer->AddComponent<PlayerComponentLastMap>();
	pPlayer->lastMap->EnterWorld(_worldId, _sn);
	pPlayer->currWorld = this;
	const auto pLastMap = pPlayer->lastMap->GetCur();

	//LOG_DEBUG("world. recv teleport. map id:" << _worldId << " world sn:" << GetSN() << " playerSn:" << pPlayer->GetPlayerSN());

	//通知客户端进入地图
	Proto::EnterWorld protoEnterWorld;
	protoEnterWorld.set_world_id(_worldId);
	pLastMap->Position.SerializeToProto(protoEnterWorld.mutable_position());
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_EnterWorld, protoEnterWorld, pPlayer);
	_adds.insert(playerSn);
}

void World::BroadcastPacket(Proto::MsgId msgId, google::protobuf::Message& proto)
{
	auto pPlayerMgr = GetComponent<PlayerManagerComponent>();
	const auto players = pPlayerMgr->GetAll();
	for (const auto& pair : *players)
	{
		//LOG_DEBUG("broadcast msgId:" << Log4Help::GetMsgIdName(msgId).c_str() << " player sn:" << pair.second->GetPlayerSN());
		MessageSystemHelp::SendPacket(msgId, proto, pair.second);
	}
}

void World::BroadcastPacket(Proto::MsgId msgId, google::protobuf::Message& proto, std::set<uint64>& players)
{
	auto pPlayerMgr = GetComponent<PlayerManagerComponent>();
	for (const auto& one : players)
	{
		const auto pPlayer = pPlayerMgr->GetPlayerBySn(one);
		if (pPlayer == nullptr)
			continue;
		//LOG_DEBUG("broadcast msgId:" << Log4Help::GetMsgIdName(msgId).c_str() << " player sn:" << one);
		MessageSystemHelp::SendPacket(msgId, proto, pPlayer);
	}
}

Player* World::GetNearestPlayer(Vector3& pos)
{
	auto players = playerMgr->GetAll();
	float dist = FLT_MAX;
	Player* player = nullptr;
	for (const auto& pair : *players)
	{
		float temp = pos.GetManhaDist(pair.second->GetCurrPos());
		if (temp < dist)
		{
			dist = temp;
			player = pair.second;
		}
	}
	return player;
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
	Proto::RoleDisappear disAppear;
	disAppear.set_sn(pPlayer->GetPlayerSN());
	BroadcastPacket(Proto::MsgId::S2C_RoleDisappear, disAppear);
	LOG_DEBUG("player disappear: sn=" << pPlayer->GetPlayerSN());
}

void World::HandleMove(Player* pPlayer, Packet* pPacket)
{
	auto proto = pPacket->ParseToProto<Proto::Move>();
	const auto positions = proto.mutable_position();
	proto.set_player_sn(pPlayer->GetPlayerSN());

	auto pMoveComponent = pPlayer->GetComponent<MoveComponent>();
	if (pMoveComponent == nullptr)
		pMoveComponent = pPlayer->AddComponent<MoveComponent>();

	std::queue<Vector3> pos;
	for (auto index = 0; index < proto.position_size(); index++)
		pos.push(Vector3(positions->Get(index)));
	const auto pComponentLastMap = pPlayer->GetComponent<PlayerComponentLastMap>();
	pMoveComponent->Update(pos, pComponentLastMap->GetCur()->Position);

	BroadcastPacket(Proto::MsgId::S2C_Move, proto);
}

void World::HandleSyncPlayerPos(Player* pPlayer, Packet* pPacket)
{
	Proto::SyncPlayerPos proto = pPacket->ParseToProto<Proto::SyncPlayerPos>();
	pPlayer->lastMap->GetCur()->Position.ParserFromProto(proto.pos());
}

void World::HandleSyncPlayerCmd(Player* pPlayer, Packet* pPacket)
{
	Proto::SyncPlayerCmd proto = pPacket->ParseToProto<Proto::SyncPlayerCmd>();
	proto.set_player_sn(pPlayer->GetPlayerSN());
	BroadcastPacket(Proto::MsgId::S2C_SyncPlayerCmd, proto);
}

void World::HandleUpdateKnapItem(Player* pPlayer, Packet* pPacket)
{
	Proto::UpdateKnapItem proto = pPacket->ParseToProto<Proto::UpdateKnapItem>();
	Proto::ItemData item = proto.item();
	auto& knap = *pPlayer->detail->pKnap;
	auto& idxMap = *pPlayer->detail->pIdxMap;
	if (idxMap.find(item.sn()) != idxMap.end())
		knap[idxMap[item.sn()]].index = item.index();
	else
	{
		idxMap.emplace(item.sn(), knap.size());
		switch (item.type())
		{
		case Proto::ItemData_ItemType_Potion:
			knap.emplace_back(ItemType::Potion, item.id(), item.index(), item.sn());
			break;
		case Proto::ItemData_ItemType_Weapon:
			knap.emplace_back(ItemType::Weapon, item.id(), item.index(), item.sn());
			break;
		default:
			break;
		}
	}
}

void World::HandleGetPlayerKnap(Player* pPlayer, Packet* pPacket)
{
	Proto::PlayerKnap proto;
	proto.set_gold(pPlayer->detail->gold);
	auto& knap = *pPlayer->detail->pKnap;
	for (auto& item : knap)
		item.SerializeToProto(proto.add_bag_items());
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_GetPlayerKnap, proto, pPlayer);
}

void World::HandleReqSyncNpc(Player* pPlayer, Packet* pPacket)
{
	Proto::ReqSyncNpc syncNpc = pPacket->ParseToProto<Proto::ReqSyncNpc>();
	int id = syncNpc.npc_id();
	uint64 sn = sn = npcs[id]->GetSN();
	syncNpc.set_npc_sn(sn);
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_ReqSyncNpc, syncNpc, pPlayer);
	Proto::SyncNpcPos syncPos;
	syncPos.set_npc_sn(sn);
	npcs[id]->GetCurrPos().SerializeToProto(syncPos.mutable_pos());
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncNpcPos, syncPos, pPlayer);
	npcs[id]->GetComponent<FsmComponent>()->GetCurrState()->Singlecast(pPlayer);
	if (!npcs[id]->GetLinkPlayer())
	{
		npcs[id]->SetLinkPlayer(pPlayer);
		Proto::ReqLinkPlayer proto;
		proto.set_npc_id(npcs[id]->GetID());
		proto.set_npc_sn(npcs[id]->GetSN());
		proto.set_linker(true);
		MessageSystemHelp::SendPacket(Proto::MsgId::S2C_ReqLinkPlayer, proto, pPlayer);
	}
}

void World::HandleSyncNpcPos(Packet* pPacket)
{
	Proto::SyncNpcPos proto = pPacket->ParseToProto<Proto::SyncNpcPos>();
	npcs[npcIdxMap[proto.npc_sn()]]->SetCurrPos(Vector3(proto.pos()));
}

void World::HandlePlayerAtkEvent(Packet* pPacket)
{
	Proto::PlayerAtkEvent proto = pPacket->ParseToProto<Proto::PlayerAtkEvent>();
	Player* player = playerMgr->GetPlayerBySn(proto.player_sn());
	if (proto.target_sn() == 0)
	{
		player->detail->hp = 1000;
		Proto::SyncEntityStatus status;
		status.set_sn(player->GetPlayerSN());
		status.set_hp(player->detail->hp);
		BroadcastPacket(Proto::MsgId::S2C_SyncEntityStatus, status);
	}
	else
	{
		Npc* npc = npcs[npcIdxMap[proto.target_sn()]];
		npc->GetDamage(player);
		Proto::SyncEntityStatus status;
		status.set_sn(npc->GetSN());
		status.set_hp(npc->hp);
		BroadcastPacket(Proto::MsgId::S2C_SyncEntityStatus, status);
	}
}

void World::HandleNpcAtkEvent(Packet* pPacket)
{
	Proto::NpcAtkEvent proto = pPacket->ParseToProto<Proto::NpcAtkEvent>();
	Player* player = playerMgr->GetPlayerBySn(proto.target_sn());
	player->GetDamage(npcs[npcIdxMap[proto.npc_sn()]]);
	Proto::SyncEntityStatus status;
	status.set_sn(player->GetPlayerSN());
	status.set_hp(player->detail->hp);
	BroadcastPacket(Proto::MsgId::S2C_SyncEntityStatus, status);
}
