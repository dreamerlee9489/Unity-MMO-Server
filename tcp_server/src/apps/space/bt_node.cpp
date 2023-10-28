#include "bt_node.h"
#include "bt_component.h"

std::default_random_engine BtActPatrol::_eng = std::default_random_engine();
std::uniform_int_distribution<int> BtActPatrol::_dis = std::uniform_int_distribution<int>(0, 3);

void BtActAttack::Enter()
{
	_npc->GetComponent<BtComponent>()->curAct = this;
	Broadcast();
}

void BtActAttack::Exit()
{
	_npc->GetComponent<BtComponent>()->curAct = nullptr;
}

void BtActAttack::Broadcast()
{
	if (_npc->target)
	{
		Proto::SyncBtAction pb;
		pb.set_id((int)BtEventId::Attack);
		pb.set_code(0);
		pb.set_npc_sn(_npc->GetSN());
		pb.set_player_sn(_npc->target->GetPlayerSN());
		_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncBtAction, pb);
	}
}

void BtActAttack::Singlecast(Player* player)
{
	if (_npc->target)
	{
		Proto::SyncBtAction pb;
		pb.set_id((int)BtEventId::Attack);
		pb.set_code(0);
		pb.set_npc_sn(_npc->GetSN());
		pb.set_player_sn(_npc->target->GetPlayerSN());
		MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncBtAction, pb, player);
	}
}

BtStatus BtActAttack::AttackTask()
{
	if (!_npc->CanAttack(_npc->target))
	{
		_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Pursue);
		return BtStatus::Suspend;
	}
	else if (!_npc->CanSee(_npc->target))
	{
		_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Idle);
		return BtStatus::Suspend;
	}
	return BtStatus::Running;
}

void BtActBirth::Enter()
{
	_npc->target = nullptr;
	_npc->linker = nullptr;
	_npc->fleeing = false;
	_npc->SetCurrPos(_npc->GetInitPos());
	_currTime = _lastTime = Global::GetInstance()->TimeTick;
}

void BtActBirth::Broadcast()
{
	Proto::SyncBtAction pb;
	pb.set_id((int)BtEventId::Birth);
	pb.set_code(0);
	pb.set_npc_sn(_npc->GetSN());
	pb.set_player_sn(0);
	_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncBtAction, pb);
}

void BtActBirth::Singlecast(Player* player)
{
	Proto::SyncBtAction pb;
	pb.set_id((int)BtEventId::Birth);
	pb.set_code(0);
	pb.set_npc_sn(_npc->GetSN());
	pb.set_player_sn(0);
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncBtAction, pb, player);
}

BtStatus BtActBirth::BirthTask()
{
	_currTime = Global::GetInstance()->TimeTick;
	if (_npc->GetWorld()->playerMgr->GetAll()->empty())
	{
		_lastTime = _currTime;
		return BtStatus::Running;
	}
	_timeElapsed = _currTime - _lastTime;
	if (_timeElapsed < 2000)
		return BtStatus::Running;
	_npc->hp = _npc->initHp;
	return BtStatus::Success;
}

void BtActDeath::Enter()
{
	_npc->target = nullptr;
	_npc->linker = nullptr;
	_currTime = _lastTime = Global::GetInstance()->TimeTick;
	Broadcast();
}

void BtActDeath::Broadcast()
{
	Proto::SyncBtAction pb;
	pb.set_id((int)BtEventId::Death);
	pb.set_code(0);
	pb.set_npc_sn(_npc->GetSN());
	pb.set_player_sn(0);
	_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncBtAction, pb);
}

void BtActDeath::Singlecast(Player* player)
{
	Proto::SyncBtAction pb;
	pb.set_id((int)BtEventId::Death);
	pb.set_code(0);
	pb.set_npc_sn(_npc->GetSN());
	pb.set_player_sn(0);
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncBtAction, pb, player);
}

BtStatus BtActDeath::DeathTask()
{
	_currTime = Global::GetInstance()->TimeTick;
	_timeElapsed = _currTime - _lastTime;
	if (_timeElapsed < 8000 || !_npc->rebirth)
		return BtStatus::Running;
	return BtStatus::Success;
}

void BtActFlee::Enter()
{
	_npc->SetNextPos(_npc->GetInitPos());
	_lastTime = _currTime = Global::GetInstance()->TimeTick;
	_npc->GetComponent<BtComponent>()->curAct = this;
	Broadcast();
}

void BtActFlee::Exit()
{
	_npc->target = nullptr;
	_npc->fleeing = false;
	_npc->GetComponent<BtComponent>()->curAct = nullptr;
}

void BtActFlee::Broadcast()
{
	Proto::SyncBtAction pb;
	pb.set_id((int)BtEventId::Flee);
	pb.set_code(0);
	pb.set_npc_sn(_npc->GetSN());
	pb.set_player_sn(0);
	_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncBtAction, pb);
}

void BtActFlee::Singlecast(Player* player)
{
	Proto::SyncBtAction pb;
	pb.set_id((int)BtEventId::Flee);
	pb.set_code(0);
	pb.set_npc_sn(_npc->GetSN());
	pb.set_player_sn(0);
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncBtAction, pb, player);
}

BtStatus BtActFlee::FleeTask()
{
	if (_npc->GetCurrPos().GetDistance(_npc->GetNextPos()) <= 1)
	{
		if (_npc->hp >= static_cast<int>(_npc->initHp * 0.7))
		{
			_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Patrol);
			return BtStatus::Suspend;
		}
		else
		{
			_currTime = Global::GetInstance()->TimeTick;
			_timeElapsed = _currTime - _lastTime;
			if (_timeElapsed < 1000)
				return BtStatus::Running;
			_lastTime = _currTime;
			_npc->hp = (std::min)(static_cast<int>(_npc->hp + _npc->initHp * 0.05), _npc->initHp);
			Proto::SyncNpcProps status;
			status.set_sn(_npc->GetSN());
			status.set_hp(_npc->hp);
			_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncNpcProps, status);
		}
	}
	return BtStatus::Running;
}

void BtActIdle::Enter()
{
	_npc->target = nullptr;
	_lastTime = _currTime = Global::GetInstance()->TimeTick;
	_npc->GetComponent<BtComponent>()->curAct = this;
	Broadcast();
}

void BtActIdle::Exit()
{
	_npc->GetComponent<BtComponent>()->curAct = nullptr;
}

void BtActIdle::Broadcast()
{
	Proto::SyncBtAction pb;
	pb.set_id((int)BtEventId::Idle);
	pb.set_code(0);
	pb.set_npc_sn(_npc->GetSN());
	pb.set_player_sn(0);
	_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncBtAction, pb);
}

void BtActIdle::Singlecast(Player* player)
{
	Proto::SyncBtAction pb;
	pb.set_id((int)BtEventId::Idle);
	pb.set_code(0);
	pb.set_npc_sn(_npc->GetSN());
	pb.set_player_sn(0);
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncBtAction, pb, player);
}

BtStatus BtActIdle::IdleTask()
{
	_currTime = Global::GetInstance()->TimeTick;
	_timeElapsed = _currTime - _lastTime;
	if (_timeElapsed < 2000)
		return BtStatus::Running;
	_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Patrol);
	return BtStatus::Suspend;
}

BtActPatrol::BtActPatrol(Npc* npc) : BtAction(npc)
{
	_task = std::bind(&BtActPatrol::PatrolTask, this);
	_index = _dis(_eng);
	_npc->SetPatrolPoint(_index);
}

void BtActPatrol::Enter()
{
	_index = _dis(_eng);
	_npc->SetPatrolPoint(_index);
	_npc->target = nullptr;
	_npc->GetComponent<BtComponent>()->curAct = this;
	Broadcast();
}

void BtActPatrol::Exit()
{
	_npc->GetComponent<BtComponent>()->curAct = nullptr;
}

void BtActPatrol::Broadcast()
{
	Proto::SyncBtAction pb;
	pb.set_id((int)BtEventId::Patrol);
	pb.set_code(_index);
	pb.set_npc_sn(_npc->GetSN());
	pb.set_player_sn(0);
	_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncBtAction, pb);
}

void BtActPatrol::Singlecast(Player* player)
{
	Proto::SyncBtAction pb;
	pb.set_id((int)BtEventId::Patrol);
	pb.set_code(_index);
	pb.set_npc_sn(_npc->GetSN());
	pb.set_player_sn(0);
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncBtAction, pb, player);
}

BtStatus BtActPatrol::PatrolTask()
{
	if (_npc->GetCurrPos().GetDistance(_npc->GetNextPos()) <= 1)
	{
		_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Idle);
		return BtStatus::Suspend;
	}
	return BtStatus::Running;
}

void BtActPursue::Enter()
{
	_npc->GetComponent<BtComponent>()->curAct = this;
	Broadcast();
}

void BtActPursue::Exit()
{
	_npc->GetComponent<BtComponent>()->curAct = nullptr;
}

void BtActPursue::Broadcast()
{
	if (_npc->target)
	{
		Proto::SyncBtAction pb;
		pb.set_id((int)BtEventId::Pursue);
		pb.set_code(0);
		pb.set_npc_sn(_npc->GetSN());
		pb.set_player_sn(_npc->target->GetPlayerSN());
		_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncBtAction, pb);
	}
}

void BtActPursue::Singlecast(Player* player)
{
	if (_npc->target)
	{
		Proto::SyncBtAction pb;
		pb.set_id((int)BtEventId::Pursue);
		pb.set_code(0);
		pb.set_npc_sn(_npc->GetSN());
		pb.set_player_sn(_npc->target->GetPlayerSN());
		MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncBtAction, pb, player);
	}
}

BtStatus BtActPursue::PursueTask()
{
	if (_npc->CanAttack(_npc->target))
	{
		_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Attack);
		return BtStatus::Suspend;
	}
	else if (!_npc->CanSee(_npc->target))
	{
		_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Idle);
		return BtStatus::Suspend;
	}
	return BtStatus::Running;
}

BtStatus BtActSense::SenseTask()
{
	if (_npc->hp == 0)
		return BtStatus::Aborted;
	if (!_npc->fleeing && _npc->hp * 1.0 / _npc->initHp * 1.0 <= 0.3)
	{
		_npc->fleeing = true;
		_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Flee, 1);
	}
	return BtStatus::Running;
}

BtStatus BtActView::ViewTask()
{
	if (!_npc->linker)
	{
		Player* player = _npc->GetWorld()->GetNearestPlayer(_npc->GetCurrPos());
		_npc->SetLinkPlayer(player);
		if (player)
		{
			Proto::ReqLinkPlayer proto;
			proto.set_npc_id(_npc->GetID());
			proto.set_npc_sn(_npc->GetSN());
			proto.set_linker(true);
			MessageSystemHelp::SendPacket(Proto::MsgId::S2C_ReqLinkPlayer, proto, player);
		}
	}
	if (!_npc->target)
	{
		for (auto& pair : *_npc->GetAllPlayer())
		{
			if (_npc->CanAttack(pair.second))
			{
				_npc->target = pair.second;
				_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Attack);
				break;
			}
			if (_npc->CanSee(pair.second))
			{
				_npc->target = pair.second;
				_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Pursue);
				break;
			}
		}
	}
	if (!_npc->fleeing && _npc->GetCurrPos().GetDistance(_npc->GetInitPos()) >= 24)
	{
		_npc->fleeing = true;
		_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Flee, 1);
	}
	return BtStatus::Running;
}