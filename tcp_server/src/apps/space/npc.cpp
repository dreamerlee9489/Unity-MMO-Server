#include "npc.h"
#include "fsm_death.h"

std::default_random_engine Npc::_realEng = std::default_random_engine();
std::uniform_real_distribution<double> Npc::_realDis = std::uniform_real_distribution<double>(0.0, 1.0);

void Npc::Awake(ResourceNpc& cfg)
{
	id = cfg.id;
	lv = cfg.level;
	hp = cfg.initHp;
	atk = cfg.initAtk;
	_nextPos = _currPos = _initPos = cfg.initPos;
}

void Npc::BackToPool()
{
	_nextPos = { 0, 0, 0 };
}

void Npc::SetLinkPlayer(Player* player)
{
	if (_linkPlayer)
	{
		Proto::ReqLinkPlayer proto;
		proto.set_npc_id(id);
		proto.set_npc_sn(_sn);
		proto.set_linker(false);
		MessageSystemHelp::SendPacket(Proto::MsgId::S2C_ReqLinkPlayer, proto, _linkPlayer);
	}
	_linkPlayer = player;
}

void Npc::SetCurrPos(const Vector3& pos)
{
	_currPos.X = pos.X;
	_currPos.Y = pos.Y;
	_currPos.Z = pos.Z;
}

void Npc::SetNextPos(const Vector3& pos)
{
	_nextPos.X = pos.X;
	_nextPos.Y = pos.Y;
	_nextPos.Z = pos.Z;
}

void Npc::SetPatrolPoint(int index)
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

bool Npc::CanSee(Player* player)
{
	if (_currPos.GetDistance(player->GetCurrPos()) <= _viewDist)
		return true;
	return false;
}

bool Npc::CanAttack(Player* player)
{
	if (_currPos.GetDistance(player->GetCurrPos()) <= _atkDist)
		return true;
	return false;
}

void Npc::GetDamage(Player* attacker)
{
	hp = max(hp - attacker->detail->atk, 0);
	Proto::SyncEntityStatus status;
	status.set_sn(_sn);
	status.set_hp(hp);
	_world->BroadcastPacket(Proto::MsgId::S2C_SyncEntityStatus, status);
	if (!hp)
		GetComponent<FsmComponent>()->ChangeState(new Death(this, attacker));		
}

std::vector<ItemData>* Npc::GetDropList(Player* player)
{
	std::vector<ItemData>* items = new std::vector<ItemData>();
	double val1 = _realDis(_realEng), val2 = _realDis(_realEng);
	_intEng = std::default_random_engine(Global::GetInstance()->TimeTick);
	if (val1 < 0.5)
	{
		_idDis = std::uniform_int_distribution<int>(1, _world->potionCfgs->size());
		_numDis = std::uniform_int_distribution<int>(1, 3);
		int count = _numDis(_intEng);
		for (int i = 0; i < count; i++)
			items->emplace_back(ItemType::Potion, _idDis(_intEng));
	}
	if (val2 < 0.5)
	{
		_idDis = std::uniform_int_distribution<int>(1, _world->weaponCfgs->size());
		items->emplace_back(ItemType::Weapon, _idDis(_intEng));
	}
	return items;
}
