#pragma once
#include "libserver/entity.h"
#include "libserver/sync_component.h"

class Packet;

class CreateWorldComponent :public SyncComponent, public Entity<CreateWorldComponent>, public IAwakeSystem<>
{
public:
	void Awake() override;
	void BackToPool() override;

private:
	int ReqCreateWorld(int worldId);

	void HandleCmdCreate(Packet* pPacket);
	void HandleAppInfoSync(Packet* pPacket);
	void HandleNetworkDisconnect(Packet* pPacket) override;
	/// <summary>
	/// appmgr找到压力最小的space线程, 发送创建公共地图消息
	/// </summary>
	/// <param name="pPacket"></param>
	void HandleRequestWorld(Packet* pPacket);
	void HandleQueryWorld(Packet* pPacket);
	void HandleBroadcastCreateWorld(Packet* pPacket);

private:
	/// <summary>
	/// 创建中的公共地图映射
	/// world id - space Id
	/// </summary>
	std::map<int, int> _creating;
	/// <summary>
	/// 已创建的公共地图映射
	/// world id - world sn
	/// </summary>
	std::map<int, uint64> _created;
	/// <summary>
	/// 已创建的副本地图映射
	/// world sn - world id
	/// </summary>
	std::map<uint64, int> _dungeons;
};

