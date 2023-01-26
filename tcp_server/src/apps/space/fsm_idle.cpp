#include "fsm_idle.h"
#include "fsm_patrol.h"
#include "fsm_pursuit.h"

void Idle::Enter()
{
	Broadcast();
}

void Idle::Execute()
{
	_currTime = Global::GetInstance()->TimeTick;
	if (_npc->GetAllPlayer()->empty())
		_lastTime = _currTime;
	else
	{
		_timeElapsed = _currTime - _lastTime;
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
		if (_timeElapsed >= 2000)
		{
			_lastTime = _currTime;
			_npc->GetComponent<FsmComponent>()->ChangeState(new Patrol(_npc));
		}
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
}

void Idle::Exit()
{
}

void Idle::Broadcast()
{
	Proto::SyncFsmState proto;
	proto.set_state((int)FsmStateType::Idle);
	proto.set_code(0);
	proto.set_player_sn(0);
	proto.set_npc_sn(_npc->GetSN());
	_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncFsmState, proto);
}

void Idle::Singlecast(Player* pPlayer)
{
	Proto::SyncFsmState proto;
	proto.set_state((int)FsmStateType::Idle);
	proto.set_code(0);
	proto.set_player_sn(0);
	proto.set_npc_sn(_npc->GetSN());
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncFsmState, proto, pPlayer);
}