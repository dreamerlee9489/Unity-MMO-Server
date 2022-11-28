#pragma once
#include "libserver/entity.h"

class Packet;

class WorldProxyLocator :public Entity<WorldProxyLocator>, public IAwakeSystem<>
{
public:
	void Awake() override;
	void BackToPool() override;

	void RegisterToLocator(int worldId, uint64 worldSn);
	void Remove(int worldId, uint64 worldSn);

	bool IsExistDungeon(uint64 worldSn);
	uint64 GetWorldSnById(int worldId);

private:
	void HandleBroadcastCreateWorld(Packet* pPacket);

private:
	std::mutex _lock;

	/// <summary>
	/// 公共地图world id - world sn映射
	/// </summary>
	std::map<int, uint64> _publics;

	/// <summary>
	/// 副本地图world sn集合(副本没有固定id, 只有临时生成的序列号)
	/// </summary>
	std::set<uint64> _worlds;
};

