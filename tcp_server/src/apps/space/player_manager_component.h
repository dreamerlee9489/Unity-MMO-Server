﻿#pragma once
#include "libserver/entity.h"
#include "libplayer/player.h"

class Player;
class PlayerManagerComponent :public Entity<PlayerManagerComponent>, public IAwakeFromPoolSystem<>
{
public:
	void Awake() override;
	void BackToPool() override;

	Player* AddPlayer(uint64 playerSn, uint64 worldSn, NetIdentify* pNetIdentify);
	Player* GetPlayerBySn(uint64 playerSn);
	Player* GetPlayerBySocket(SOCKET socket);
	void RemovePlayerBySn(uint64 playerSn);
	void RemoveAllPlayers(NetIdentify* pNetIdentify);

	int OnlineSize() const;
	std::map<uint64, Player*>* GetAll();

private:
	std::map<uint64, Player*> _players;
	std::map<SOCKET, Player*> _sockets;
};

