#ifndef BT_ACTION_ATTACK
#define BT_ACTION_ATTACK
#include "bt_action.h"

class BtActAttack : public BtAction
{
public:
	BtActAttack(Npc* npc) : BtAction(npc)
	{
		_task = std::bind(&BtActAttack::AttackTask, this);
	}

	~BtActAttack() = default;

	void Enter() override
	{
		_npc->GetComponent<BtComponent>()->curAct = this;
		Broadcast();
	}

	void Exit() override
	{
		_npc->GetComponent<BtComponent>()->curAct = nullptr;
	}

	void Broadcast() override
	{
		if (_npc->target)
		{
			Proto::SyncBtAction pb;
			pb.set_id((int)BtEventId::Attack);
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
			pb.set_id((int)BtEventId::Attack);
			pb.set_code(0);
			pb.set_npc_sn(_npc->GetSN());
			pb.set_player_sn(_npc->target->GetPlayerSN());
			MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncBtAction, pb, player);
		}
	}

private:
	BtStatus AttackTask()
	{
		if (!_npc->CanAttack(_npc->target))
		{
			_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Pursue);
			return BtStatus::Suspend;
		}
		else if (!_npc->CanSee(_npc->target))
		{
			_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Idle);
			return BtStatus::Suspend;
		}
		//LOG_DEBUG(_npc->id << " AttackTask()");
		return BtStatus::Running;
	}
};

#endif // !BT_ACTION_ATTACK
