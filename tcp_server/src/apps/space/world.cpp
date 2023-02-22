#include "world.h"
#include "libplayer/cmd_none.h"
#include "libplayer/cmd_move.h"
#include "libplayer/cmd_attack.h"
#include "libplayer/cmd_pick.h"
#include "libplayer/cmd_teleport.h"
#include "libplayer/cmd_observe.h"
#include "libplayer/cmd_pvp.h"

WorldGather* World::_worldMgr = nullptr;

void World::Awake(int worldId)
{
	//LOG_DEBUG("create world. id:" << worldId << " sn:" << _sn << " space app id:" << Global::GetAppIdFromSN(_sn));
	_worldId = worldId;
	playerMgr = AddComponent<PlayerManagerComponent>();
	syncWorldTimer = AddTimer(0, 10, false, 0, BindFunP0(this, &World::SyncWorldToGather));
	syncAppearTimer = AddTimer(0, 1, false, 0, BindFunP0(this, &World::SyncAppearTimer));
	if (!_worldMgr)
		_worldMgr = ComponentHelp::GetGlobalEntitySystem()->GetComponent<WorldGather>();

	worldCfg = ResourceHelp::GetResourceManager()->Worlds->GetResource(_worldId);
	std::vector<ResourceNpc>& npcCfgs = *worldCfg->GetNpcCfgs();
	potionCfgs = worldCfg->GetPotionCfgs();
	weaponCfgs = worldCfg->GetWeaponCfgs();
	for (auto& cfg : npcCfgs)
	{
		Npc* npc = GetSystemManager()->GetEntitySystem()->AddComponent<Npc>(cfg);
		npc->SetWorld(this);
		npc->SetAllPlayer(playerMgr->GetAll());
		npc->AddComponent<BtComponent>();
		npc->AddComponent<MoveComponent>();
		npcIdxMap.emplace(npc->GetSN(), npcs.size());
		npcs.push_back(npc);
		if (worldCfg->GetType() == ResourceWorldType::Public)
			npc->rebirth = true;
	}

	// message
	auto pMsgSystem = GetSystemManager()->GetMessageSystem();
	pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_NetworkDisconnect, BindFunP1(this, &World::HandleNetworkDisconnect));
	pMsgSystem->RegisterFunction(this, Proto::MsgId::G2S_SyncPlayer, BindFunP1(this, &World::HandleSyncPlayer));
	pMsgSystem->RegisterFunction(this, Proto::MsgId::C2S_SyncNpcPos, BindFunP1(this, &World::HandleSyncNpcPos));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::G2S_RequestSyncPlayer, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleRequestSyncPlayer));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::G2S_RemovePlayer, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleG2SRemovePlayer));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2S_PlayerMove, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandlePlayerMove));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2S_NpcMove, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleNpcMove));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2S_UpdateKnapItem, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleUpdateKnapItem));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2S_UpdateKnapGold, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleUpdateKnapGold));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2S_GetPlayerKnap, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleGetPlayerKnap));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2S_SyncPlayerPos, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleSyncPlayerPos));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2S_SyncPlayerCmd, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleSyncPlayerCmd));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2S_ReqSyncPlayer, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleReqSyncPlayer));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2S_ReqNpcInfo, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleReqNpcInfo));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2S_SyncBtAction, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleSyncBtAction));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2S_PlayerAtkEvent, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandlePlayerAtkEvent));
	pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2S_NpcAtkEvent, BindFunP1(this, &World::GetPlayer), BindFunP2(this, &World::HandleNpcAtkEvent));
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
	const auto pTagSocket = pTags->GetTagValue(TagType::Socket);
	if (pTagSocket)
	{
		Player* pPlayer = playerMgr->GetPlayerBySocket(pTagSocket->KeyInt64);
		return pPlayer;
	}
	const auto pTagPlayer = pTags->GetTagValue(TagType::Player);
	if (pTagPlayer)
	{
		Player* pPlayer = playerMgr->GetPlayerBySn(pTagPlayer->KeyInt64);
		return pPlayer;
	}
	return nullptr;
}

void World::PlayerDisappear(Player* pPlayer)
{
	pPlayer->GetComponent<CmdComponent>()->ResetCmd();
	pPlayer->RemoveComponent<CmdComponent>();
	for (auto& enemy : npcs)
	{
		if (playerMgr->GetAll()->empty())
		{
			enemy->target = nullptr;
			enemy->GetComponent<BtComponent>()->AddEvent(BtEventId::Birth, 10);
		}
		else if (enemy->target == pPlayer)
		{
			enemy->target = nullptr;
			enemy->GetComponent<BtComponent>()->AddEvent(BtEventId::Idle, 10);
		}
		if (enemy->linker == pPlayer)
		{
			//enemy->GetComponent<FsmComponent>()->ResetState();			
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
		_worldMgr->RegistPlayer(pPlayer->GetPlayerSN(), nullptr);
		Proto::RoleDisappear protoDis;
		protoDis.set_sn(pPlayer->GetPlayerSN());
		BroadcastPacket(Proto::MsgId::S2C_RoleDisappear, protoDis);
		pPlayerMgr->RemovePlayerBySn(pTagPlayer->KeyInt64);
		PlayerDisappear(pPlayer);
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

void World::HandlePlayerMove(Player* pPlayer, Packet* pPacket)
{
	auto proto = pPacket->ParseToProto<Proto::EntityMove>();
	const auto points = proto.mutable_points();
	auto moveComp = pPlayer->GetComponent<MoveComponent>();
	if (moveComp == nullptr)
		moveComp = pPlayer->AddComponent<MoveComponent>();
	moveComp->moveSpeed = 5.56f;
	std::queue<Vector3> que;
	for (auto index = 0; index < proto.points_size(); ++index)
		que.push(Vector3(points->Get(index)));
	const auto lastMapComp = pPlayer->GetComponent<PlayerComponentLastMap>();
	moveComp->SetPath(std::move(que), lastMapComp->GetCur()->Position);
	BroadcastPacket(Proto::MsgId::S2C_PlayerMove, proto);
}

void World::HandleNpcMove(Player* pPlayer, Packet* pPacket)
{
	auto proto = pPacket->ParseToProto<Proto::EntityMove>();
	const auto points = proto.mutable_points();
	Npc* npc = npcs[npcIdxMap[proto.sn()]];
	auto moveComp = npc->GetComponent<MoveComponent>();
	moveComp->moveSpeed = proto.running() ? 5.56f : 1.56f;
	std::queue<Vector3> que;
	for (auto index = 0; index < proto.points_size(); ++index)
		que.push(Vector3(points->Get(index)));
	moveComp->SetPath(std::move(que), npc->GetCurrPos());
	BroadcastPacket(Proto::MsgId::S2C_NpcMove, proto);
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

inline void AddRoleToAppear(Player* pPlayer, Proto::AllRoleAppear& protoAppear)
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
			AddRoleToAppear(pPlayer, protoNewAppear);
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
			AddRoleToAppear(role, protoOther);
		}

		if (protoOther.roles_size() > 0)
			BroadcastPacket(Proto::MsgId::S2C_AllRoleAppear, protoOther, _adds);
		_adds.clear();
	}
}

/// <summary>
/// 将player添加到world实例中, 通知客户端加载地图
/// </summary>
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

	_worldMgr->RegistPlayer(playerSn, pPlayer);
	pPlayer->ParserFromProto(playerSn, proto.player());
	pPlayer->AddComponent<CmdComponent>();
	pPlayer->detail = pPlayer->AddComponent<PlayerComponentDetail>();
	pPlayer->lastMap = pPlayer->AddComponent<PlayerComponentLastMap>();
	pPlayer->lastMap->EnterWorld(_worldId, _sn);
	pPlayer->curWorld = this;
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
	float dist = 9999;
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

	playerMgr->RemovePlayerBySn(pPlayer->GetPlayerSN());
	if (worldCfg->GetType() == ResourceWorldType::Dungeon && playerMgr->GetAll()->empty())
	{
		RemoveTimer(syncWorldTimer);
		RemoveTimer(syncAppearTimer);
		Proto::DungeonDisapper pb;
		pb.set_world_sn(_sn);
		MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_DungeonDisapper, pb, nullptr);
	}

	// 通知其他玩家	
	PlayerDisappear(pPlayer);
	Proto::RoleDisappear disAppear;
	disAppear.set_sn(pPlayer->GetPlayerSN());
	BroadcastPacket(Proto::MsgId::S2C_RoleDisappear, disAppear);
	//LOG_DEBUG("player disappear: sn=" << pPlayer->GetPlayerSN());
}

void World::HandleSyncPlayerPos(Player* pPlayer, Packet* pPacket)
{
	Proto::SyncPlayerPos proto = pPacket->ParseToProto<Proto::SyncPlayerPos>();
	pPlayer->lastMap->GetCur()->Position.ParserFromProto(proto.pos());
}

void World::HandleSyncPlayerCmd(Player* pPlayer, Packet* pPacket)
{
	Proto::SyncPlayerCmd proto = pPacket->ParseToProto<Proto::SyncPlayerCmd>();
	switch ((CmdType)proto.type())
	{
	case CmdType::None:
		pPlayer->GetComponent<CmdComponent>()->ChangeCmd(new NoneCommand(pPlayer));
		break;
	case CmdType::Move:
		pPlayer->GetComponent<CmdComponent>()->ChangeCmd(new MoveCommand(pPlayer, Vector3::CreateByProto(proto.point())));
		break;
	case CmdType::Attack:
		pPlayer->GetComponent<CmdComponent>()->ChangeCmd(new AttackCommand(pPlayer, npcs[npcIdxMap[proto.target_sn()]]));
		break;
	case CmdType::Pick:
		pPlayer->GetComponent<CmdComponent>()->ChangeCmd(new PickCommand(pPlayer, proto.target_sn(), Vector3::CreateByProto(proto.point())));
		break;
	case CmdType::Teleport:
		pPlayer->GetComponent<CmdComponent>()->ChangeCmd(new TeleportCommand(pPlayer, proto.target_sn()));
		break;
	case CmdType::Observe:
		pPlayer->GetComponent<CmdComponent>()->ChangeCmd(new ObserveCommand(pPlayer, playerMgr->GetPlayerBySn(proto.target_sn())));
		break;
	case CmdType::Pvp:
		pPlayer->GetComponent<CmdComponent>()->ChangeCmd(new PvpCommand(pPlayer, playerMgr->GetPlayerBySn(proto.target_sn())));
		break;
	default:
		break;
	}
}

void World::HandleReqNpcInfo(Player* pPlayer, Packet* pPacket)
{
	Proto::ReqNpcInfo proto = pPacket->ParseToProto<Proto::ReqNpcInfo>();
	int id = proto.npc_id();
	proto.set_npc_sn(npcs[id]->GetSN());
	npcs[id]->GetCurrPos().SerializeToProto(proto.mutable_pos());
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_ReqNpcInfo, proto, pPlayer);
}

void World::HandleSyncBtAction(Player* pPlayer, Packet* pPacket)
{
	Proto::SyncBtAction proto = pPacket->ParseToProto<Proto::SyncBtAction>();
	int id = proto.id();
	npcs[id]->GetComponent<BtComponent>()->SyncAction(pPlayer);
	if (!npcs[id]->linker)
	{
		npcs[id]->SetLinkPlayer(pPlayer);
		Proto::ReqLinkPlayer proto;
		proto.set_npc_id(npcs[id]->GetID());
		proto.set_npc_sn(npcs[id]->GetSN());
		proto.set_linker(true);
		MessageSystemHelp::SendPacket(Proto::MsgId::S2C_ReqLinkPlayer, proto, pPlayer);
	}
}

void World::HandleReqSyncPlayer(Player* pPlayer, Packet* pPacket)
{
	Proto::ReqSyncPlayer proto = pPacket->ParseToProto<Proto::ReqSyncPlayer>();
	Player* target = playerMgr->GetPlayerBySn(proto.player_sn());
	Command* cmd = target->GetComponent<CmdComponent>()->GetCurrCmd();
	if (cmd)
		cmd->Singlecast(pPlayer);
}

void World::HandleSyncNpcPos(Packet* pPacket)
{
	Proto::SyncNpcPos proto = pPacket->ParseToProto<Proto::SyncNpcPos>();
	npcs[npcIdxMap[proto.npc_sn()]]->SetCurrPos(Vector3(proto.pos()));
}

void World::HandlePlayerAtkEvent(Player* pPlayer, Packet* pPacket)
{
	Proto::PlayerAtkEvent proto = pPacket->ParseToProto<Proto::PlayerAtkEvent>();
	Player* defender = playerMgr->GetPlayerBySn(proto.target_sn());
	if (defender)
		defender->GetDamage(pPlayer);
	else
	{
		if (proto.target_sn() != 0)
			npcs[npcIdxMap[proto.target_sn()]]->GetDamage(pPlayer);
		else
		{
			pPlayer->detail->hp = 1000;
			Proto::SyncPlayerProps status;
			status.set_sn(pPlayer->GetPlayerSN());
			status.set_hp(pPlayer->detail->hp);
			BroadcastPacket(Proto::MsgId::S2C_SyncPlayerProps, status);
		}
	}
}

void World::HandleNpcAtkEvent(Player* pPlayer, Packet* pPacket)
{
	Proto::NpcAtkEvent proto = pPacket->ParseToProto<Proto::NpcAtkEvent>();
	pPlayer->GetDamage(npcs[npcIdxMap[proto.npc_sn()]]);
}

void World::HandleUpdateKnapItem(Player* pPlayer, Packet* pPacket)
{
	Proto::UpdateKnapItem proto = pPacket->ParseToProto<Proto::UpdateKnapItem>();
	pPlayer->UpdateKnapItem(proto.item());
}

void World::HandleUpdateKnapGold(Player* pPlayer, Packet* pPacket)
{
	Proto::UpdateKnapGold proto = pPacket->ParseToProto<Proto::UpdateKnapGold>();
	pPlayer->detail->gold = proto.gold();
}

void World::HandleGetPlayerKnap(Player* pPlayer, Packet* pPacket)
{
	pPlayer->GetPlayerKnap();
}
