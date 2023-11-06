#include "player.h"
#include "player_component.h"
#include "space/world.h"
#include "libserver/message_system_help.h"

void Player::Awake(NetIdentify* pIdentify, std::string account)
{
	_account = account;
	_playerSn = 0;
	_player.Clear();		

	_tagKey.Clear();
	_tagKey.AddTag(TagType::Account, _account);

	if (pIdentify != nullptr)
		_socketKey.CopyFrom(pIdentify->GetSocketKey());
	
	// 登录成功，修改网络底层的标识
	MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_NetworkListenKey, this);
}

void Player::Awake(NetIdentify* pIdentify, uint64 playerSn, uint64 worldSn)
{
	_account = "";
	_playerSn = playerSn;
	_player.Clear();
	
	_tagKey.Clear();
	_tagKey.AddTag(TagType::Player, playerSn);
	_tagKey.AddTag(TagType::Entity, worldSn);

	if (pIdentify != nullptr)
		_socketKey.CopyFrom(pIdentify);

	// space进程调用，但Space不需要修改网络标识
	// 登录成功，修改网络底层的标识
	//MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_NetworkListenKey, this);
}

void Player::BackToPool()
{
	_account = "";
	_name = "";
	_playerSn = 0;
	_player.Clear();

	_socketKey.Clear();
	_tagKey.Clear();
}

void Player::GetDamage(Npc* enemy)
{
	detail->hp = (std::max)(detail->hp - enemy->atk, 0);
	Proto::SyncPlayerProps status;
	status.set_sn(_playerSn);
	status.set_hp(detail->hp);
	curWorld->BroadcastPacket(Proto::MsgId::S2C_SyncPlayerProps, status);
	auto& teamMap = curWorld->GetWorldMgr()->teamMap;
	if (teamMap.find(_playerSn) != teamMap.end())
	{
		auto& members = teamMap[_playerSn]->GetMembers();
		for (uint64& sn : members)
		{
			Player* member = curWorld->GetWorldMgr()->GetPlayerBySn(sn);
			if (sn != _playerSn && member->curWorld != curWorld)
				MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncPlayerProps, status, member);
		}
	}
}

void Player::GetDamage(Player* atker)
{
	detail->hp = (std::max)(detail->hp - atker->detail->atk, 0);
	if (detail->hp == 0)
	{
		atker->GetComponent<CmdComponent>()->ResetCmd();
		Proto::SyncPlayerCmd proto;
		proto.set_type((int)CmdType::Death);
		proto.set_player_sn(_playerSn);
		proto.set_target_sn(atker->GetPlayerSN());
		curWorld->BroadcastPacket(Proto::MsgId::S2C_SyncPlayerCmd, proto);
	}
	Proto::SyncPlayerProps status;
	status.set_sn(_playerSn);
	status.set_hp(detail->hp);
	curWorld->BroadcastPacket(Proto::MsgId::S2C_SyncPlayerProps, status);
	auto& teamMap = curWorld->GetWorldMgr()->teamMap;
	if (teamMap.find(_playerSn) != teamMap.end())
	{
		auto& members = teamMap[_playerSn]->GetMembers();
		for (uint64& sn : members)
		{
			Player* member = curWorld->GetWorldMgr()->GetPlayerBySn(sn);
			if (sn != _playerSn && member->curWorld != curWorld)
				MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncPlayerProps, status, member);
		}
	}
}

void Player::UpdateKnapItem(const Proto::ItemData& item)
{
	auto& knap = *detail->pKnap;
	auto& idxMap = *detail->pIdxMap;
	if (idxMap.find(item.sn()) != idxMap.end())
	{
		knap[idxMap[item.sn()]].knapType = (KnapType)item.knaptype();
		knap[idxMap[item.sn()]].index = item.index();
	}
	else
	{
		idxMap.emplace(item.sn(), knap.size());
		knap.emplace_back(ItemData().ParserFromProto(item));
	}
}

void Player::GetPlayerKnap()
{
	Proto::PlayerKnap proto;
	proto.set_gold(detail->gold);
	auto& knap = *detail->pKnap;
	for (auto& item : knap)
		item.SerializeToProto(proto.add_items());
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_GetPlayerKnap, proto, this);
}

bool Player::CanSee(Vector3& point)
{
	if (lastMap->GetCur()->Position.GetDistance(point) <= _viewDist)
		return true;
	return false;
}

bool Player::CanAttack(Vector3& point)
{
	if (lastMap->GetCur()->Position.GetDistance(point) <= _atkDist)
		return true;
	return false;
}

void Player::ParseFromStream(const uint64 playerSn, std::stringstream* pOpStream)
{
	_playerSn = playerSn;
	_player.ParseFromIstream(pOpStream);
}

void Player::ParserFromProto(const uint64 playerSn, const Proto::Player& proto)
{
	_playerSn = playerSn;
	_player.CopyFrom(proto);
	_name = _player.name();
	
	// 内存中修改数据
	for (auto& pair : _components)
	{
		auto pPlayerComponent = dynamic_cast<PlayerComponent*>(pair.second);
		if (pPlayerComponent == nullptr)
			continue;

		pPlayerComponent->ParserFromProto(proto);
	}
}

void Player::SerializeToProto(Proto::Player* pProto)
{
	// 基础数据
	pProto->CopyFrom(_player);

	// 内存中修改数据
	for (auto& pair : _components)
	{
		auto pPlayerComponent = dynamic_cast<PlayerComponent*>(pair.second);
		if (pPlayerComponent == nullptr)
			continue;

		pPlayerComponent->SerializeToProto(pProto);
	}
}

void Player::Singlecast(Proto::MsgId msgId, google::protobuf::Message& proto, Player* pPlayer)
{
	MessageSystemHelp::SendPacket(msgId, proto, pPlayer);
}

void Player::Broadcast(Proto::MsgId msgId, google::protobuf::Message& proto, std::map<uint64, Player*>& playerMap)
{
	for (auto& pair : playerMap)
		MessageSystemHelp::SendPacket(msgId, proto, pair.second);
}
