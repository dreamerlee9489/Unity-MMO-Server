#ifndef BT_ACTION_IDLE
#define BT_ACTION_IDLE
#include "bt_action.h"

class BtActIdle : public BtAction
{
public:
	BtActIdle(Npc* npc) : BtAction(npc) 
	{
		_task = std::bind(&BtActIdle::IdleTask, this);
	}

	~BtActIdle() = default;

	BtEventId GetEventId() override { return BtEventId::Idle; }

	void Enter() override 
	{ 
		_npc->target = nullptr;
		_lastTime = _currTime = Global::GetInstance()->TimeTick;
		_npc->GetComponent<BtComponent>()->curAct = this;
		Broadcast(); 
	}

	void Exit() override 
	{
		_npc->GetComponent<BtComponent>()->curAct = nullptr;
	}

	void Broadcast() override
	{
		Proto::SyncBtAction pb;
		pb.set_id((int)BtEventId::Idle);
		pb.set_code(0);
		pb.set_npc_sn(_npc->GetSN());
		pb.set_player_sn(0);
		_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncBtAction, pb);
	}

	void Singlecast(Player* player) override
	{
		Proto::SyncBtAction pb;
		pb.set_id((int)BtEventId::Idle);
		pb.set_code(0);
		pb.set_npc_sn(_npc->GetSN());
		pb.set_player_sn(0);
		MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncBtAction, pb, player);
	}

private:
	BtStatus IdleTask()
	{
		_currTime = Global::GetInstance()->TimeTick;
		_timeElapsed = _currTime - _lastTime;
		if (_timeElapsed < 2000)
			return BtStatus::Running;
		_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Patrol);
		return BtStatus::Suspend;
	}
};

#endif // !BT_ACTION_IDLE
