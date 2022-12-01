#include "attack.h"
#include "pursuit.h"
#include "idle.h"

void Attack::Enter()
{
	_owner->SetSpeed(0);
	_lastMap = _target->GetComponent<PlayerComponentLastMap>();
	SyncState();
}

void Attack::Execute()
{
	if (_lastMap->GetCur())
	{
		float dist = _owner->GetCurrPos().GetDistance(_lastMap->GetCur()->Position);
		if (dist >= 2 * _owner->AttackDist)
			_owner->GetComponent<AIComponent>()->ChangeState(new Idle(_owner));
		else if (dist >= _owner->AttackDist)
			_owner->GetComponent<AIComponent>()->ChangeState(new Pursuit(_owner, _target));
	}

}

void Attack::Exit()
{
}