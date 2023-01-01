#include "fsm_death.h"

void Death::Enter()
{
	Broadcast();
	Singlecast(_target);
}

void Death::Execute()
{
}

void Death::Exit()
{
}

void Death::Broadcast()
{
	Proto::FsmSyncState proto;
	proto.set_state((int)FsmStateType::Death);
	proto.set_code(0);
	proto.set_enemy_id(_owner->GetID());
	proto.set_player_sn(0);
	_owner->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_FsmSyncState, proto);
}

void Death::Singlecast(Player* pPlayer)
{
	std::vector<DropItem>* items = _owner->GetDropList();
	Proto::ItemList proto;
	proto.set_enemy_id(_owner->GetID());
	for (DropItem& item : *items)
	{
		Proto::ItemData* data = proto.add_items();
		data->set_id(item.id);
		data->set_num(item.num);
		switch (item.type)
		{
		case ItemType::None:
			data->set_type(Proto::ItemData_ItemType_None);
			break;
		case ItemType::Potion:
			data->set_type(Proto::ItemData_ItemType_Potion);
			break;
		case ItemType::Weapon:
			data->set_type(Proto::ItemData_ItemType_Weapon);
			break;
		default:
			break;
		}
	}
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_ItemList, proto, pPlayer);
	delete items;
}
