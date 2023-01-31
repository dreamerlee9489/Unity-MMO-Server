#ifndef BT_ACTION_VIEW
#define BT_ACTION_VIEW
#include "bt_action.h"

class BtActView : public BtAction
{
public:
	BtActView(Npc* npc) : BtAction(npc) 
	{
		_task = std::bind(&BtActView::ViewTask, this);
	}

	~BtActView() = default;

	void Broadcast() override {}

	void Singlecast(Player* player) override {}

	void Enter() override {}

	void Exit() override {}

	BtStatus ViewTask()
	{
		if (!_npc->linker)
		{
			Player* player = _npc->GetWorld()->GetNearestPlayer(_npc->GetCurrPos());
			_npc->SetLinkPlayer(player);
			if (player)
			{
				Proto::ReqLinkPlayer proto;
				proto.set_npc_id(_npc->GetID());
				proto.set_npc_sn(_npc->GetSN());
				proto.set_linker(true);
				MessageSystemHelp::SendPacket(Proto::MsgId::S2C_ReqLinkPlayer, proto, player);
			}
		}
		if (!_npc->target)
		{
			for (auto& pair : *_npc->GetAllPlayer())
			{
				if (_npc->CanAttack(pair.second))
				{
					_npc->target = pair.second;
					_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Attack);
					break;
				}
				if (_npc->CanSee(pair.second))
				{
					_npc->target = pair.second;
					_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Pursue);
					break;
				}
			}
		}
		if (!_npc->fleeing && _npc->GetCurrPos().GetDistance(_npc->GetInitPos()) >= 24)
		{
			_npc->fleeing = true;
			_npc->GetComponent<BtComponent>()->AddEvent(BtEventId::Flee, 1);
		}
		return BtStatus::Running;
	}
};

#endif // !BT_ACTION_VIEW
