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
