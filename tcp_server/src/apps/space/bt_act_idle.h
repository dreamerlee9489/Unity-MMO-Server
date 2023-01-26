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

	void Enter() override 
	{ 
		_npc->target = nullptr;
		_lastTime = _currTime = Global::GetInstance()->TimeTick;
		Broadcast(); 
	}

	void Exit() override {}

private:
	BtStatus IdleTask()
	{
		//LOG_DEBUG(_npc->id << " IdleTask");
		if (!_npc->target)
		{
			for (auto& pair : *_npc->GetAllPlayer())
			{
				if (_npc->CanSee(pair.second))
				{
					_npc->target = pair.second;
					_npc->GetComponent<BtComponent>()->events.emplace(BtEventId::Pursue);
					return BtStatus::Success;
				}
			}
		}
		_currTime = Global::GetInstance()->TimeTick;
		_timeElapsed = _currTime - _lastTime;
		return _timeElapsed > 2000 ? BtStatus::Failure : BtStatus::Running;
	}
};

#endif // !BT_ACTION_IDLE
