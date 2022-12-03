#include "patrol.h"
#include "pursuit.h"

Patrol::Patrol(AIEnemy* owner, Player* target) : AIState(owner, target)
{
	_eng = std::default_random_engine(_owner->GetID());
	_players = owner->GetWorld()->GetPlayerManager()->GetAll();
}

void Patrol::Enter()
{
	_index = _dis(_eng);
	_owner->SetPatrolPoint(_index);
	_owner->SetSpeed(_owner->WalkSpeed);
	SyncState();
}

void Patrol::Execute()
{
	_currTime = Global::GetInstance()->TimeTick;
	_timeElapsed = _currTime - _lastTime;
	for (auto pair : *_players)
	{
		float dist = _owner->GetCurrPos().GetDistance(pair.second->GetComponent<PlayerComponentLastMap>()->GetCur()->Position);
		if (dist <= 8)
		{
			_owner->GetComponent<AIComponent>()->ChangeState(new Pursuit(_owner, pair.second));
			break;
		}
	}
	if (_timeElapsed >= 10000)
	{
		_lastTime = _currTime;
		_index = _dis(_eng);
		_owner->SetPatrolPoint(_index);
		SyncState();
	}
}

void Patrol::Exit()
{
}

void Patrol::SyncState()
{
	Proto::FsmChangeState proto;
	proto.set_state((int)AIStateType::Patrol);
	proto.set_code(_index);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(0);
	_owner->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_FsmChangeState, proto);
}