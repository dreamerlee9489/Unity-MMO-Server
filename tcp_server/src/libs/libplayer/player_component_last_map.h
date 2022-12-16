#pragma once
#include "libserver/component.h"

#include "player_component.h"
#include "libserver/vector3.h"
#include "libserver/system.h"

/// <summary>
/// 上次登录的地图id/sn/坐标(可能是公共地图也可能是副本)
/// </summary>
struct LastWorld
{
	int WorldId{ 0 };
	uint64 WorldSn{ 0 };
	Vector3 Position{ 0, 0,0 };

	LastWorld(const int worldId, const uint64 worldSn, const Vector3 pos);

	LastWorld(Net::LastWorld proto);

	void SerializeToProto(Net::LastWorld* pProto) const;
};

/// <summary>
/// 玩家上次登录的公共地图/副本地图数据
/// </summary>
class PlayerComponentLastMap :public Component<PlayerComponentLastMap>, public IAwakeFromPoolSystem<>, public PlayerComponent
{
public:
	void Awake() override;
	void BackToPool() override;

	void ParserFromProto(const Net::Player& proto) override;
	void SerializeToProto(Net::Player* pProto) override;

	LastWorld* GetLastPublicMap() const;
	LastWorld* GetLastDungeon() const;
	LastWorld* GetCur() const;
	/// <summary>
	/// 修改LastMap属性
	/// </summary>
	void EnterWorld(int worldId, uint64 worldSn);
	void SetCurWorld(int worldId);

protected:
	void EnterDungeon(int worldId, uint64 worldSn, Vector3 position);

private:
	// 一个是公共地图的位置，一个是副本位置
	LastWorld* _pPublic{ nullptr };
	LastWorld* _pDungeon{ nullptr };

	int _curWorldId{ 0 };
};

