#include "ai_enemy.h"

void AIEnemy::Awake(int id, Vector3 pos)
{
	_id = id;
	_initPos = pos;
	_currPos = pos;
	_nextPos = pos;
}

void AIEnemy::BackToPool()
{
	_id = 0;
	_nextPos = { 0, 0, 0 };
}

void AIEnemy::SetLinkPlayer(Player* player)
{
	if (_linkPlayer)
	{
		Proto::RequestLinkPlayer proto;
		proto.set_enemy_id(_id);
		proto.set_islinker(false);
		MessageSystemHelp::SendPacket(Proto::MsgId::S2C_RequestLinkPlayer, proto, _linkPlayer);
	}
	_linkPlayer = player;
}

void AIEnemy::SetCurrPos(const Vector3& pos)
{
	_currPos.X = pos.X;
	_currPos.Y = pos.Y;
	_currPos.Z = pos.Z;
}

void AIEnemy::SetNextPos(const Vector3& pos)
{
	_nextPos.X = pos.X;
	_nextPos.Y = pos.Y;
	_nextPos.Z = pos.Z;
}

void AIEnemy::SetPatrolPoint(int index)
{
	switch (index)
	{
	case 0:
		_nextPos.X = _initPos.X - 5;
		_nextPos.Z = _initPos.Z - 5;
		break;
	case 1:
		_nextPos.X = _initPos.X - 5;
		_nextPos.Z = _initPos.Z + 5;
		break;
	case 2:
		_nextPos.X = _initPos.X + 5;
		_nextPos.Z = _initPos.Z - 5;
		break;
	case 3:
		_nextPos.X = _initPos.X + 5;
		_nextPos.Z = _initPos.Z + 5;
		break;
	}
}

bool AIEnemy::CanSee(Player* player)
{
	if (_currPos.GetDistance(player->GetCurrPos()) <= _viewDist)
		return true;
	return false;
}

bool AIEnemy::CanAttack(Player* player)
{
	if (_currPos.GetDistance(player->GetCurrPos()) <= _atkDist)
		return true;
	return false;
}
