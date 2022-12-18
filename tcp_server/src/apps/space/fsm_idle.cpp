#include "fsm_idle.h"
#include "fsm_patrol.h"
#include "fsm_pursuit.h"

Idle::Idle(AIEnemy* owner, Player* target) : FsmState(owner, target)
{
	_type = FsmStateType::Idle;
}

void Idle::Enter()
{
	BroadcastState();
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
			Proto::RequestLinkPlayer proto;
			proto.set_enemy_id(_owner->GetID());
			proto.set_islinker(true);
			MessageSystemHelp::SendPacket(Proto::MsgId::S2C_RequestLinkPlayer, proto, player);
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

void Idle::BroadcastState()
{
	Proto::FsmSyncState proto;
	proto.set_state((int)FsmStateType::Idle);
	proto.set_code(0);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(0);
	_owner->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_FsmSyncState, proto);
}

void Idle::SendState(Player* pPlayer)
{
	Proto::FsmSyncState proto;
	proto.set_state((int)FsmStateType::Idle);
	proto.set_code(0);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(0);
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_FsmSyncState, proto, pPlayer);
}