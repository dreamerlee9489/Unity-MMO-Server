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
	pMsgSystem->RegisterFunction(this, Proto::MsgId::C2S_SyncFsmState, BindFunP1(this, &World::HandleSyncFsmState));
	pMsgSystem->RegisterFunction(this, Proto::MsgId::C2S_PushEnemyPos, BindFunP1(this, &World::HandlePushEnemyPos));
	pMsgSystem->RegisterFunction(this, Proto::MsgId::C2S_AtkAnimEvent, BindFunP1(this, &World::HandleAtkAnimEvent));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::G2S_RequestSyncPlayer, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleRequestSyncPlayer));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::G2S_RemovePlayer, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleG2SRemovePlayer));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2S_Move, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleMove));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2S_PushPlayerPos, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandlePushPlayerPos));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2S_SyncPlayerCmd, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleSyncPlayerCmd));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2S_RequestSyncEnemy, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleRequestSyncEnemy));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2S_UpdateKnapItem, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleUpdateKnapItem));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2S_GetPlayerKnap, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleGetPlayerKnap));

	ResourceWorld* worldCfg = ResourceHelp::GetResourceManager()->Worlds->GetResource(_worldId);
	std::vector<ResourceEnemy>* _enemyCfgs = worldCfg->GetEnemies();
	potions = worldCfg->GetPotions();
	weapons = worldCfg->GetWeapons();
	for (auto& cfg : *_enemyCfgs)
	{
		AIEnemy* enemy = GetSystemManager()->GetEntitySystem()->AddComponent<AIEnemy>(cfg);
		enemy->SetWorld(this);
		enemy->SetAllPlayer(playerMgr->GetAll());
		enemy->AddComponent<FsmComponent>();
		enemies.push_back(enemy);
	}
}

void World::BackToPool()
{
	_adds.clear();
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

		for (auto& enemy : enemies)
		{
			if (enemy->GetLinkPlayer() == pPlayer)
			{
				enemy->GetComponent<FsmComponent>()->ResetState();
				Player* target = GetNearestPlayer(enemy->GetCurrPos());
				enemy->SetLinkPlayer(target);
				if (target)
				{
					Proto::RequestLinkPlayer proto;
					proto.set_enemy_id(enemy->GetID());
					proto.set_linker(true);
					MessageSystemHelp::SendPacket(Proto::MsgId::S2C_RequestLinkPlayer, proto, target);
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

void World::HandlePushPlayerPos(Player* pPlayer, Packet* pPacket)
{
	Proto::PushPlayerPos proto = pPacket->ParseToProto<Proto::PushPlayerPos>();
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
	auto* pKnap = pPlayer->detail->pKnap;
	bool inKnap = false;
	for (auto& iter = pKnap->begin(); iter != pKnap->end(); ++iter)
	{
		if ((*iter).key == item.key())
		{
			inKnap = true;
			(*iter).num += item.num();
			(*iter).index = item.index();
			break;
		}
	}
	if (!inKnap)
	{
		switch (item.type())
		{
		case Proto::ItemData_ItemType_Potion:
			pKnap->emplace_back(ItemType::Potion, item.id(), item.num(), item.index(), item.key());
			break;
		case Proto::ItemData_ItemType_Weapon:
			pKnap->emplace_back(ItemType::Weapon, item.id(), item.num(), item.index(), item.key());
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
	{
		Proto::ItemData* itemData = proto.add_itemsinbag();
		itemData->set_id(item.id);
		itemData->set_num(item.num);
		itemData->set_index(item.index);
		itemData->set_key(item.key);
		switch (item.type)
		{
		case ItemType::Potion:
			itemData->set_type(Proto::ItemData_ItemType_Potion);
			break;
		case ItemType::Weapon:
			itemData->set_type(Proto::ItemData_ItemType_Weapon);
			break;
		default:
			break;
		}
	}
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_GetPlayerKnap, proto, pPlayer);
}

void World::HandleRequestSyncEnemy(Player* pPlayer, Packet* pPacket)
{
	Proto::RequestSyncEnemy proto = pPacket->ParseToProto<Proto::RequestSyncEnemy>();
	int id = proto.enemy_id();
	if (!enemies[id]->GetLinkPlayer())
	{
		enemies[id]->SetLinkPlayer(pPlayer);
		Proto::RequestLinkPlayer proto;
		proto.set_enemy_id(enemies[id]->GetID());
		proto.set_linker(true);
		MessageSystemHelp::SendPacket(Proto::MsgId::S2C_RequestLinkPlayer, proto, pPlayer);
	}
	Proto::PushEnemyPos protoEnemy;
	protoEnemy.set_id(id);
	enemies[id]->GetCurrPos().SerializeToProto(protoEnemy.mutable_pos());
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_PushEnemyPos, protoEnemy, pPlayer);
	enemies[id]->GetComponent<FsmComponent>()->GetCurrState()->Singlecast(pPlayer);
}

void World::HandlePushEnemyPos(Packet* pPacket)
{
	Proto::PushEnemyPos proto = pPacket->ParseToProto<Proto::PushEnemyPos>();
	enemies[proto.id()]->SetCurrPos(Vector3(proto.pos()));
}

void World::HandleSyncFsmState(Packet* pPacket)
{
	Proto::SyncFsmState proto = pPacket->ParseToProto<Proto::SyncFsmState>();
	Player* player = playerMgr->GetPlayerBySn(proto.player_sn());
	enemies[proto.enemy_id()]->GetComponent<FsmComponent>()->SyncState(proto, player);
}

void World::HandleAtkAnimEvent(Packet* pPacket)
{
	Proto::AtkAnimEvent proto = pPacket->ParseToProto<Proto::AtkAnimEvent>();
	Player* player = playerMgr->GetPlayerBySn(proto.player_sn());
	if (proto.enemy_id() == -1)
	{
		proto.set_curr_hp(player->detail->hp = 1000);
		BroadcastPacket(Proto::MsgId::S2C_AtkAnimEvent, proto);
		return;
	}
	AIEnemy* enemy = enemies[proto.enemy_id()];
	if (proto.atkenemy())
	{
		proto.set_curr_hp(enemy->GetDamage(player));
		BroadcastPacket(Proto::MsgId::S2C_AtkAnimEvent, proto);
	}
	else
	{
		proto.set_curr_hp(player->GetDamage(enemy));
		BroadcastPacket(Proto::MsgId::S2C_AtkAnimEvent, proto);
	}
}
