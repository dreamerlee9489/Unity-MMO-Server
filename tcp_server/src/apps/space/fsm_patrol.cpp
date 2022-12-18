#include "fsm_patrol.h"
#include "fsm_pursuit.h"

Patrol::Patrol(AIEnemy* owner, Player* target) : FsmState(owner, target)
{
	_currTime = Global::GetInstance()->TimeTick;
	_type = FsmStateType::Patrol;
	_eng = std::default_random_engine(_currTime % UINT32_MAX + _owner->GetID());
}

void Patrol::Enter()
{
	_index = _dis(_eng);
	_owner->SetPatrolPoint(_index);
	BroadcastState();
}

void Patrol::Execute()
{
	if (!_owner->GetLinkPlayer())
	{
		Player* player = _owner->GetWorld()->GetNearestPlayer(_owner->GetCurrPos());
		_owner->SetLinkPlayer(player);
		Proto::RequestLinkPlayer proto;
		proto.set_enemy_id(_owner->GetID());
		proto.set_islinker(true);
		MessageSystemHelp::SendPacket(Proto::MsgId::S2C_RequestLinkPlayer, proto, player);
	}
	if (_owner->GetCurrPos().GetDistance(_owner->GetNextPos()) <= 1)
		_owner->GetComponent<FsmComponent>()->ChangeState(new Idle(_owner));
	else
	{
		for (auto& pair : *_owner->GetAllPlayer())
		{
			if (_owner->CanSee(pair.second))
			{
				_owner->GetComponent<FsmComponent>()->ChangeState(new Pursuit(_owner, pair.second));
				break;
			}
		}
	}
}

void Patrol::Exit()
{
}

void Patrol::BroadcastState()
{
	Proto::FsmSyncState proto;
	proto.set_state((int)FsmStateType::Patrol);
	proto.set_code(_index);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(0);
	_owner->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_FsmSyncState, proto);
}

void Patrol::SendState(Player* pPlayer)
{
	Proto::FsmSyncState proto;
	proto.set_state((int)FsmStateType::Patrol);
	proto.set_code(_index);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(0);
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_FsmSyncState, proto, pPlayer);
}