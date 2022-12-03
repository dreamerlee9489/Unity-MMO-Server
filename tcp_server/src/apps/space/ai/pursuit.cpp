#include "pursuit.h"
#include "attack.h"

void Pursuit::Enter()
{
	_owner->SetSpeed(_owner->RunSpeed);
	_lastMap = _target->GetComponent<PlayerComponentLastMap>();
	_owner->SetNextPos(_lastMap->GetCur()->Position);
	SyncState();
}

void Pursuit::Execute()
{
	if (_lastMap->GetCur() && _owner->GetDistToNext() >= _owner->StopDist)
		_owner->SetNextPos(_lastMap->GetCur()->Position);
	else
		_owner->GetComponent<AIComponent>()->ChangeState(new Attack(_owner, _target));
}

void Pursuit::Exit()
{
	_owner->SetSpeed(_owner->WalkSpeed);
}

void Pursuit::SyncState()
{
	Proto::FsmChangeState proto;
	proto.set_state((int)AIStateType::Pursuit);
	proto.set_code(0);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(_target->GetPlayerSN());
	_owner->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_FsmChangeState, proto);
}