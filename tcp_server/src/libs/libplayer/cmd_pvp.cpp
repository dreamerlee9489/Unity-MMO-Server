#include "cmd_pvp.h"

void PvpCommand::Enter()
{
	Command::Enter();
}

CmdState PvpCommand::Execute()
{
	return state;
}

void PvpCommand::Exit()
{
	Command::Exit();
}

void PvpCommand::Broadcast()
{
	Proto::SyncPlayerCmd proto;
	proto.set_type((int)CmdType::Pvp);
	proto.set_player_sn(_owner->GetPlayerSN());
	proto.set_target_sn(_target->GetPlayerSN());
	_owner->curWorld->BroadcastPacket(Proto::MsgId::S2C_SyncPlayerCmd, proto);
}

void PvpCommand::Singlecast(Player* player)
{
	Proto::SyncPlayerCmd proto;
	proto.set_type((int)CmdType::Pvp);
	proto.set_player_sn(_owner->GetPlayerSN());
	proto.set_target_sn(_target->GetPlayerSN());
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncPlayerCmd, proto, player);
}
