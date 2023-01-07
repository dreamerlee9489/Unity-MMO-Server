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
	if (_owner->GetAllPlayer()->empty())
		_lastTime = _currTime;
	else
	{
		_timeElapsed = _currTime - _lastTime;
		if (!_owner->GetLinkPlayer())
		{
			Player* player = _owner->GetWorld()->GetNearestPlayer(_owner->GetCurrPos());
			_owner->SetLinkPlayer(player);
			Proto::ReqLinkPlayer proto;
			proto.set_npc_id(_owner->GetID());
			proto.set_npc_sn(_owner->GetSN());
			proto.set_linker(true);
			MessageSystemHelp::SendPacket(Proto::MsgId::S2C_ReqLinkPlayer, proto, player);
		}
		if (_timeElapsed >= 2000)
		{
			_lastTime = _currTime;
			_owner->GetComponent<FsmComponent>()->ChangeState(new Patrol(_owner));
		}
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
	proto.set_npc_sn(_owner->GetSN());
	_owner->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncFsmState, proto);
}

void Idle::Singlecast(Player* pPlayer)
{
	Proto::SyncFsmState proto;
	proto.set_state((int)FsmStateType::Idle);
	proto.set_code(0);
	proto.set_player_sn(0);
	proto.set_npc_sn(_owner->GetSN());
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncFsmState, proto, pPlayer);
}