#include "teleport_object.h"

void TeleportObject::Awake(const int worldId, const uint64 playerSn, bool isPublic)
{
	FlagWorld.Flag = TeleportFlagType::None;
	FlagWorld.Value = 0;

	FlagPlayerSync.Flag = TeleportFlagType::None;
	FlagPlayerSync.Value = false;

	_targetWorldId = worldId;
	_playerSn = playerSn;
	this->isPublic = isPublic;
}

void TeleportObject::BackToPool()
{
	FlagWorld.Flag = TeleportFlagType::None;
	FlagWorld.Value = 0;

	FlagPlayerSync.Flag = TeleportFlagType::None;
	FlagPlayerSync.Value = false;

	_targetWorldId = 0;
	_playerSn = 0;
}

int TeleportObject::GetTargetWorldId() const
{
	return _targetWorldId;
}

uint64 TeleportObject::GetPlayerSN() const
{
	return _playerSn;
}
