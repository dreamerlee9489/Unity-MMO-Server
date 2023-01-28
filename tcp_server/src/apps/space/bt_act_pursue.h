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

	void Enter() override 
	{ 
		_npc->GetComponent<BtComponent>()->curAct = this;
		Broadcast(); 
		//LOG_DEBUG("pursue enter");
	}

	void Exit() override 
	{
		_npc->GetComponent<BtComponent>()->curAct = nullptr;
		//LOG_DEBUG("pursue exit");
	}

	void Broadcast() override
	{
		if (_npc->target)
		{
			Proto::SyncBtAction pb;
			pb.set_id((int)BtEventId::Pursue);
			pb.set_code(0);
			pb.set_npc_sn(_npc->GetSN());
			pb.set_player_sn(_npc->target->GetPlayerSN());
			_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncBtAction, pb);
		}
	}

	void Singlecast(Player* player) override
	{
		if (_npc->target)
		{
			Proto::SyncBtAction pb;
			pb.set_id((int)BtEventId::Pursue);
			pb.set_code(0);
			pb.set_npc_sn(_npc->GetSN());
			pb.set_player_sn(_npc->target->GetPlayerSN());
			MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncBtAction, pb, player);
		}
	}

private:
	BtStatus PursueTask()
	{
		if (_npc->CanAttack(_npc->target))
		{
			_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Attack);
			return BtStatus::Suspend;
		}
		else if (!_npc->CanSee(_npc->target))
		{
			_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Idle);
			return BtStatus::Suspend;
		}
		return BtStatus::Running;
	}
};

#endif // !BT_ACTION_PURSUE
