#ifndef BT_ACTION_BIRTH
#define BT_ACTION_BIRTH
#include "bt_action.h"

class BtActBirth : public BtAction
{
public:
	BtActBirth(Npc* npc) : BtAction(npc)
	{
		_task = std::bind(&BtActBirth::BirthTask, this);
	}

	~BtActBirth() = default;

	BtEventId GetEventId() override { return BtEventId::Birth; }

	void Enter() override 
	{
		_npc->target = nullptr;
		_npc->linker = nullptr;
		_npc->fleeing = false;
		_currTime = _lastTime = Global::GetInstance()->TimeTick;
	}

	void Exit() override {}

	BtStatus BirthTask()
	{
		_currTime = Global::GetInstance()->TimeTick;
		if (_npc->GetWorld()->playerMgr->GetAll()->empty())
		{
			_lastTime = _currTime;
			return BtStatus::Running;
		}
		_timeElapsed = _currTime - _lastTime;
		if (_timeElapsed < 2000)
			return BtStatus::Running;
		_npc->hp = _npc->initHp;
		return BtStatus::Success;
	}

	void Broadcast() override
	{
		Proto::SyncBtAction pb;
		pb.set_id((int)BtEventId::Birth);
		pb.set_code(0);
		pb.set_npc_sn(_npc->GetSN());
		pb.set_player_sn(0);
		_npc->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncBtAction, pb);
	}

	void Singlecast(Player* player) override
	{
		Proto::SyncBtAction pb;
		pb.set_id((int)BtEventId::Birth);
		pb.set_code(0);
		pb.set_npc_sn(_npc->GetSN());
		pb.set_player_sn(0);
		MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncBtAction, pb, player);
	}
};

#endif // !BT_ACTION_BIRTH
