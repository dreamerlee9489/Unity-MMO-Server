#include "fsm_patrol.h"
#include "fsm_pursuit.h"

std::default_random_engine Patrol::_eng = std::default_random_engine();
std::uniform_int_distribution<int> Patrol::_dis = std::uniform_int_distribution<int>(0, 3);

void Patrol::Enter()
{
	_index = _dis(_eng);
	_npc->SetPatrolPoint(_index);
	Broadcast();
}

void Patrol::Execute()
{
	if (!_npc->GetLinkPlayer())
	{
		Player* player = _npc->GetWorld()->GetNearestPlayer(_npc->GetCurrPos());
		_npc->SetLinkPlayer(player);
		Proto::ReqLinkPlayer proto;
		proto.set_npc_id(_npc->GetID());
		proto.set_npc_sn(_npc->GetSN());
		proto.set_linker(true);
		MessageSystemHelp::SendPacket(Proto::MsgId::S2C_ReqLinkPlayer, proto, player);
	}
	if (_npc->GetCurrPos().GetDistance(_npc->GetNextPos()) <= 1)
		_npc->GetComponent<FsmComponent>()->ChangeState(new Idle(_npc));
	else
	{
		for (auto& pair : *_npc->GetAllPlayer())
		{
			if (_npc->CanSee(pair.second))
			{
				_npc->GetComponent<FsmComponent>()->ChangeState(new Pursuit(_npc, pair.second));
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
	proto.set_npc_sn(_npc->GetSN());
	proto.set_player_sn(0);
	_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncFsmState, proto);
}

void Patrol::Singlecast(Player* pPlayer)
{
	Proto::SyncFsmState proto;
	proto.set_state((int)FsmStateType::Patrol);
	proto.set_code(_index);
	proto.set_npc_sn(_npc->GetSN());
	proto.set_player_sn(0);
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncFsmState, proto, pPlayer);
}