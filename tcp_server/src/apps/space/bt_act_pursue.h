#ifndef BT_ACTION_PURSUE
#define BT_ACTION_PURSUE
#include "bt_action.h"

class BtActPursue : public BtAction
{
public:
	BtActPursue(Npc* npc) : BtAction(npc)
	{
		_task = std::bind(&BtActPursue::PursueTask, this);
	}

	~BtActPursue() = default;

	void Broadcast() override
	{
		Proto::SyncBtAction pb;
		pb.set_id((int)BtEventId::Pursue);
		pb.set_code(0);
		pb.set_npc_sn(_npc->GetSN());
		pb.set_player_sn(_npc->target->GetPlayerSN());
		_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncBtAction, pb);
	}

	void Singlecast(Player* player) override
	{
		Proto::SyncBtAction pb;
		pb.set_id((int)BtEventId::Pursue);
		pb.set_code(0);
		pb.set_npc_sn(_npc->GetSN());
		pb.set_player_sn(_npc->target->GetPlayerSN());
		MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncBtAction, pb, player);
	}

	void Enter() override { Broadcast(); }

	void Exit() override {}

private:
	BtStatus PursueTask()
	{
		LOG_DEBUG(_npc->id << " pursue...");
		return BtStatus::Running;
	}
};

#endif // !BT_ACTION_PURSUE
