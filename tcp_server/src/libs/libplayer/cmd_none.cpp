#include "cmd_none.h"

void NoneCommand::Broadcast()
{
	Proto::SyncPlayerCmd proto;
	proto.set_type((int)CmdType::None);
	proto.set_player_sn(_owner->GetPlayerSN());
	proto.set_target_sn(0);
	_owner->curWorld->BroadcastPacket(Proto::MsgId::S2C_SyncPlayerCmd, proto);
}

void NoneCommand::Singlecast(Player* player)
{
	Proto::SyncPlayerCmd proto;
	proto.set_type((int)CmdType::None);
	proto.set_player_sn(_owner->GetPlayerSN());
	proto.set_target_sn(0);
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncPlayerCmd, proto, player);
}
