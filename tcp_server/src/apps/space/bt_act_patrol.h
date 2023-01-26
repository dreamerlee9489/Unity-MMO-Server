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

	void Enter() override { Broadcast(); }

	void Exit() override {}

private:
	int _index = 0;
	static std::default_random_engine _eng;
	static std::uniform_int_distribution<int> _dis;

	BtStatus PatrolTask()
	{
		//LOG_DEBUG(_npc->id << " PatrolTask");
		if (!_npc->GetLinkPlayer())
		{
			Player* player = _npc->GetWorld()->GetNearestPlayer(_npc->GetCurrPos());
			_npc->SetLinkPlayer(player);
			Proto::ReqLinkPlayer proto;
			proto.set_npc_id(_npc->GetID());
			proto.set_npc_sn(_npc->GetSN());
			proto.set_linker(true);
			MessageSystemHelp::SendPacket(Proto::MsgId::S2C_ReqLinkPlayer, proto, player);
		}
		if (_npc->GetCurrPos().GetDistance(_npc->GetNextPos()) <= 1)
			_npc->GetComponent<BtComponent>()->events.emplace(BtEventId::Idle);
		else
		{
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
		}
		return BtStatus::Running;
	}
};

std::default_random_engine BtActPatrol::_eng = std::default_random_engine();
std::uniform_int_distribution<int> BtActPatrol::_dis = std::uniform_int_distribution<int>(0, 3);

#endif // !BT_ACTION_PATROL
