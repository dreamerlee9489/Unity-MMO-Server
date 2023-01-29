#ifndef BT_ACTION_DEATH
#define BT_ACTION_DEATH
#include "bt_action.h"

class BtActDeath : public BtAction
{
public:
	BtActDeath(Npc* npc) : BtAction(npc)
	{
		_task = std::bind(&BtActDeath::DeathTask, this);		
	}

	~BtActDeath() = default;

	BtEventId GetEventId() override { return BtEventId::Birth; }

	void Enter() override 
	{
		_npc->target = nullptr;
		_npc->linker = nullptr;
		_currTime = _lastTime = Global::GetInstance()->TimeTick;
		Broadcast();
	}

	void Exit() override {}

	BtStatus DeathTask()
	{
		_currTime = Global::GetInstance()->TimeTick;
		_timeElapsed = _currTime - _lastTime;
		if (_timeElapsed < 8000 || !_npc->rebirth)
			return BtStatus::Running;
		_npc->SetCurrPos(_npc->GetInitPos());
		return BtStatus::Success;
	}

	void Broadcast() override
	{
		Proto::SyncBtAction pb;
		pb.set_id((int)BtEventId::Death);
		pb.set_code(0);
		pb.set_npc_sn(_npc->GetSN());
		pb.set_player_sn(0);
		_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncBtAction, pb);
	}

	void Singlecast(Player* player) override
	{
		Proto::SyncBtAction pb;
		pb.set_id((int)BtEventId::Death);
		pb.set_code(0);
		pb.set_npc_sn(_npc->GetSN());
		pb.set_player_sn(0);
		MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncBtAction, pb, player);
	}
};

#endif // !BT_ACTION_DEATH
