#pragma once
#include "libserver/component.h"
#include "libserver/system.h"

class WorldProxy;
class Player;
class TeleportObject;

struct TeleportStruct
{
	TeleportObject* _pObj = nullptr;
	WorldProxy* _proxy = nullptr;

	TeleportStruct() {}
	TeleportStruct(TeleportObject* pObj, WorldProxy* proxy)
	{
		_pObj = pObj;
		_proxy = proxy;
	}
};

class WorldComponentTeleport :public Component<WorldComponentTeleport>, public IAwakeFromPoolSystem<>
{
public:
	void Awake() override;
	void BackToPool() override;

	bool IsTeleporting(Player* pPlayer);
	std::map<uint64, TeleportStruct>& GetTeleportObjects() { return _objects; }

	void CreateTeleportObject(int worldId, Player* pPlayer, bool isPublic);
	void HandleBroadcastCreateWorldProxy(int worldId, uint64 worldSn);
	void BroadcastSyncPlayer(uint64 playerSn);
	void Teleport(WorldProxy* pWorldProxy, TeleportObject* pObj, Player* pPlayer);

protected:
	void CreateWorldFlag(WorldProxy* pWorldProxy, int targetWorldId, TeleportObject* pObj);
	void CreateSyncFlag(WorldProxy* pWorldProxy, TeleportObject* pObj);
	bool Check(TeleportObject* pObj);

private:
	// <playersn, obj>
	std::map<uint64, TeleportStruct> _objects;
};