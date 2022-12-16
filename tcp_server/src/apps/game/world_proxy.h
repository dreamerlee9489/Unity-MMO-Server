#pragma once
#include "libserver/entity.h"
#include "libserver/socket_object.h"

#include "libplayer/world_base.h"

class Player;

/// <summary>
/// 主要解决跳转地图切换网络连接的开销以及space线程的耦合
/// </summary>
class WorldProxy :public IWorld, public Entity<WorldProxy>, public IAwakeFromPoolSystem<int, uint64>
{
public:
	void Awake(int worldId, uint64 lastWorldSn) override;
	void BackToPool() override;
	/// <summary>
	/// 向World实例发送player数据
	/// </summary>
	void SendPacketToWorld(const Net::MsgId msgId, ::google::protobuf::Message& proto, Player* pPlayer) const;
	/// <summary>
	/// 向World实例发送player数据
	/// </summary>
	void SendPacketToWorld(const Net::MsgId msgId, Player* pPlayer) const;
	/// <summary>
	/// 将数据包拷贝发送到space进程
	/// </summary>
	void CopyPacketToWorld(Player* pPlayer, Packet* pPacket) const;

private:
	Player* GetPlayer(NetIdentify* pIdentify);

	void HandleNetworkDisconnect(Packet* pPacket);
	/// <summary>
	/// 将player数据添加到代理并转发到World实例
	/// 通知旧地图, 移除player数据
	/// </summary>
	void HandleTeleport(Packet* pPacket);
	void HandleTeleportAfter(Player* pPlayer, Packet* pPacket);
	void HandleBroadcastCreateWorldProxy(Packet* pPacket);
	void HandleC2GEnterWorld(Player* pPlayer, Packet* pPacket);
	void HandleS2GSyncPlayer(Player* pPlayer, Packet* pPacket);
	/// <summary>
	/// 不处理消息, 直接转发
	/// </summary>
	void HandleDefaultFunction(Packet* pPacket);

private:
	int _spaceAppId{ 0 };
};

