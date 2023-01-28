#ifndef BT_ACTION_PATROL
#define BT_ACTION_PATROL
#include "bt_action.h"

class BtActPatrol : public BtAction
{
public:
	BtActPatrol(Npc* npc) : BtAction(npc)
	{
		_task = std::bind(&BtActPatrol::PatrolTask, this);
		_index = _dis(_eng);
		_npc->SetPatrolPoint(_index);
	}

	~BtActPatrol() = default;

	BtEventId GetEventId() override { return BtEventId::Patrol; }

	void Enter() override 
	{
		_index = _dis(_eng);
		_npc->SetPatrolPoint(_index);
		_npc->target = nullptr;
		_npc->GetComponent<BtComponent>()->curAct = this;
		Broadcast(); 
		//LOG_DEBUG("patrol enter");
	}

	void Exit() override 
	{
		_npc->GetComponent<BtComponent>()->curAct = nullptr;
		//LOG_DEBUG("patrol exit");
	}

	void Broadcast() override
	{
		Proto::SyncBtAction pb;
		pb.set_id((int)BtEventId::Patrol);
		pb.set_code(_index);
		pb.set_npc_sn(_npc->GetSN());
		pb.set_player_sn(0);
		_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncBtAction, pb);
	}

	void Singlecast(Player* player) override
	{
		Proto::SyncBtAction pb;
		pb.set_id((int)BtEventId::Patrol);
		pb.set_code(_index);
		pb.set_npc_sn(_npc->GetSN());
		pb.set_player_sn(0);
		MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncBtAction, pb, player);
	}

private:
	int _index = 0;
	static std::default_random_engine _eng;
	static std::uniform_int_distribution<int> _dis;

	BtStatus PatrolTask()
	{
		if (_npc->GetCurrPos().GetDistance(_npc->GetNextPos()) <= 1)
		{
			_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Idle);
			return BtStatus::Suspend;
		}
		return BtStatus::Running;
	}
};

std::default_random_engine BtActPatrol::_eng = std::default_random_engine();
std::uniform_int_distribution<int> BtActPatrol::_dis = std::uniform_int_distribution<int>(0, 3);

#endif // !BT_ACTION_PATROL
