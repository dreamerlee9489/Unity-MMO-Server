﻿#pragma once
#include "libserver/common.h"
#include "libserver/socket_object.h"
#include "libserver/entity.h"
#include "libserver/system.h"

class Player :public Entity<Player>, public NetIdentify,
	virtual public IAwakeFromPoolSystem<NetIdentify*, std::string>,
	virtual public IAwakeFromPoolSystem<NetIdentify*, uint64, uint64>
{
public:
	void Awake(NetIdentify* pIdentify, std::string account) override;
	void Awake(NetIdentify* pIdentify, uint64 playerSn, uint64 worldSn) override;
	void BackToPool() override;

	std::string GetAccount() const;
	std::string GetName() const;
	uint64 GetPlayerSN() const;

	Net::Player& GetPlayerProto();

	void ParseFromStream(uint64 playerSn, std::stringstream* pOpStream);
	void ParserFromProto(uint64 playerSn, const Net::Player& proto);
	void SerializeToProto(Net::Player* pProto) const;

protected:
	std::string _account{ "" };
	std::string _name{ "" };

	uint64 _playerSn{ 0 };
	Net::Player _player;
};

