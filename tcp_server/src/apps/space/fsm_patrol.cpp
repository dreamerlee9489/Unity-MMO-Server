#include "fsm_patrol.h"
#include "fsm_pursuit.h"

std::default_random_engine Patrol::_eng = std::default_random_engine();
std::uniform_int_distribution<int> Patrol::_dis = std::uniform_int_distribution<int>(0, 3);

void Patrol::Enter()
{
	_index = _dis(_eng);
	_owner->SetPatrolPoint(_index);
	Broadcast();
}

void Patrol::Execute()
{
	if (!_owner->GetLinkPlayer())
	{
		Player* player = _owner->GetWorld()->GetNearestPlayer(_owner->GetCurrPos());
		_owner->SetLinkPlayer(player);
		Proto::RequestLinkPlayer proto;
		proto.set_enemy_id(_owner->GetID());
		proto.set_linker(true);
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

void Patrol::Broadcast()
{
	Proto::SyncFsmState proto;
	proto.set_state((int)FsmStateType::Patrol);
	proto.set_code(_index);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(0);
	_owner->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncFsmState, proto);
}

void Patrol::Singlecast(Player* pPlayer)
{
	Proto::SyncFsmState proto;
	proto.set_state((int)FsmStateType::Patrol);
	proto.set_code(_index);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(0);
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncFsmState, proto, pPlayer);
}