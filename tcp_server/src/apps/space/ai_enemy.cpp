#include "ai_enemy.h"
#include "fsm_death.h"

std::default_random_engine AIEnemy::_realEng = std::default_random_engine();
std::uniform_real_distribution<double> AIEnemy::_realDis = std::uniform_real_distribution<double>(0.0, 1.0);

void AIEnemy::Awake(ResourceEnemy& cfg)
{
	lv = cfg.level;
	hp = cfg.initHp;
	atk = cfg.initAtk;
	_id = cfg.id;
	_nextPos = _currPos = _initPos = cfg.initPos;
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
		proto.set_linker(false);
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

int AIEnemy::GetDamage(Player* attacker)
{
	hp = max(hp - attacker->detail->atk, 0);
	if (!hp)
		GetComponent<FsmComponent>()->ChangeState(new Death(this, attacker));
	return hp;
}

std::vector<DropItem>* AIEnemy::GetDropList(Player* player)
{
	std::vector<DropItem>* items = new std::vector<DropItem>();
	int exp = (atk + lv) * 5, gold = (atk + lv) * 10;
	player->detail->xp += exp;
	player->detail->gold += gold;
	items->emplace_back(ItemType::None, DROP_EXPR, exp, 0, 0);
	items->emplace_back(ItemType::None, DROP_GOLD, gold, 0, 0);
	double val1 = _realDis(_realEng), val2 = _realDis(_realEng);
	_intEng = std::default_random_engine(Global::GetInstance()->TimeTick);
	if (val1 < 0.5)
	{
		_idDis = std::uniform_int_distribution<int>(1, _world->potions->size());
		_numDis = std::uniform_int_distribution<int>(1, 3);
		items->emplace_back(ItemType::Potion, _idDis(_intEng), _numDis(_intEng), 0, 0);
	}
	if (val2 < 0.5)
	{
		_idDis = std::uniform_int_distribution<int>(1, _world->weapons->size());
		items->emplace_back(ItemType::Weapon, _idDis(_intEng), 1, 0, 0);
	}
	return items;
}
