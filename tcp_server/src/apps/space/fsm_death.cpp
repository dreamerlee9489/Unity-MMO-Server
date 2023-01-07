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
	Proto::SyncFsmState proto;
	proto.set_state((int)FsmStateType::Death);
	proto.set_code(0);
	proto.set_npc_sn(_owner->GetSN());
	proto.set_player_sn(0);
	_owner->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncFsmState, proto);
}

void Death::Singlecast(Player* pPlayer)
{
	Proto::SyncPlayerCmd cmd;
	cmd.set_type(0);
	cmd.set_player_sn(pPlayer->GetPlayerSN());
	_owner->GetWorld()->BroadcastPacket(Proto::MsgId::S2C_SyncPlayerCmd, cmd);
	int exp = (_owner->atk + _owner->lv) * 5, gold = (_owner->atk + _owner->lv) * 10;
	pPlayer->detail->xp += exp;
	pPlayer->detail->gold += gold;
	std::vector<ItemData>* items = _owner->GetDropList(pPlayer);
	Proto::DropItemList list;
	list.set_npc_sn(_owner->GetSN());
	list.set_exp(exp);
	list.set_gold(gold);
	for (ItemData& item : *items)
		item.SerializeToProto(list.add_items());
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_DropItemList, list, pPlayer);
	delete items;
}
