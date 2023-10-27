#include "bt_act_sense.h"

BtActSense::BtActSense(Npc* npc) : BtAction(npc)
{
	_task = std::bind(&BtActSense::SenseTask, this);
}

BtStatus BtActSense::SenseTask()
{
	if (_npc->hp == 0)
		return BtStatus::Aborted;
	if (!_npc->fleeing && _npc->hp * 1.0 / _npc->initHp * 1.0 <= 0.3)
	{
		_npc->fleeing = true;
		_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Flee, 1);
	}
	return BtStatus::Running;
}
