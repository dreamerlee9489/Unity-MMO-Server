﻿#include "player_manager_component.h"
#include "libserver/message_system_help.h"

void PlayerManagerComponent::Awake()
{
}

void PlayerManagerComponent::BackToPool()
{
	// 当一个Mgr被销毁的时候，它不应该有任何玩家
	if (!_players.empty())
		LOG_ERROR("called PlayerManagerComponent BackToPool. but it has player.");

	_players.clear();
}

/// <summary>
/// 
/// </summary>
/// <param name="playerSn"></param>
/// <param name="worldSn"></param>
/// <param name="pNetIdentify"></param>
/// <returns></returns>
Player* PlayerManagerComponent::AddPlayer(const uint64 playerSn, uint64 worldSn, NetIdentify* pNetIdentify)
{
	const auto iter = _players.find(playerSn);
	if (iter != _players.end())
	{
		std::cout << "AddPlayer error." << std::endl;
		return nullptr;
	}

	const auto pPlayer = GetSystemManager()->GetEntitySystem()->AddComponent<Player>(pNetIdentify, playerSn, worldSn);
	_players[playerSn] = pPlayer;
	_sockets[pPlayer->GetSocket()] = pPlayer;
	return pPlayer;
}

Player* PlayerManagerComponent::GetPlayerBySn(const uint64 playerSn)
{
	const auto iter = _players.find(playerSn);
	if (iter == _players.end())
		return nullptr;

	return iter->second;
}

Player* PlayerManagerComponent::GetPlayerBySocket(SOCKET socket)
{
	const auto iter = _sockets.find(socket);
	if (iter == _sockets.end())
		return nullptr;

	return iter->second;
}

void PlayerManagerComponent::RemovePlayerBySn(const uint64 playerSn)
{
	const auto iter = _players.find(playerSn);
	if (iter == _players.end())
		return;

	Player* pPlayer = iter->second;
	_players.erase(playerSn);
	_sockets.erase(pPlayer->GetSocket());
	GetSystemManager()->GetEntitySystem()->RemoveComponent(pPlayer);
}

void PlayerManagerComponent::RemoveAllPlayers(NetIdentify* pNetIdentify)
{
	_sockets.clear();
	auto iter = _players.begin();
	while (iter != _players.end())
	{
		auto pPlayer = iter->second;
		if (pPlayer->GetSocketKey()->Socket != pNetIdentify->GetSocketKey()->Socket)
		{
			++iter;
			continue;
		}

		iter = _players.erase(iter);

		// save
		Proto::SavePlayer protoSave;
		protoSave.set_player_sn(pPlayer->GetPlayerSN());
		pPlayer->SerializeToProto(protoSave.mutable_player());
		MessageSystemHelp::SendPacket(Proto::MsgId::G2DB_SavePlayer, protoSave, APP_DB_MGR);

		GetSystemManager()->GetEntitySystem()->RemoveComponent(pPlayer);
	}
}

int PlayerManagerComponent::OnlineSize() const
{
	return _players.size();
}

std::map<uint64, Player*>* PlayerManagerComponent::GetAll()
{
	return &_players;
}
