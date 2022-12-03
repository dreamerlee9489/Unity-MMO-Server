#include "attack.h"
#include "pursuit.h"
#include "idle.h"

void Attack::Enter()
{
	_lastMap = _target->GetComponent<PlayerComponentLastMap>();
	_owner->SetSpeed(_owner->WalkSpeed);
	SyncState();
}

void Attack::Execute()
{
	if (_lastMap->GetCur())
	{
		float dist = _owner->GetCurrPos().GetDistance(_lastMap->GetCur()->Position);
		if (dist >= _owner->PursuitDist)
			_owner->GetComponent<AIComponent>()->ChangeState(new Idle(_owner));
		else if (dist >= _owner->AttackDist)
			_owner->GetComponent<AIComponent>()->ChangeState(new Pursuit(_owner, _target));
	}
}

void Attack::Exit()
{
}

void Attack::SyncState()
{
	Proto::FsmChangeState proto;
	proto.set_state((int)AIStateType::Attack);
	proto.set_code(0);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(_target->GetPlayerSN());
	_owner->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_FsmChangeState, proto);
}