#include "cmd_teleport.h"

void TeleportCommand::Enter()
{
	Command::Enter();
}

CmdState TeleportCommand::Execute()
{
	return state = CmdState::Finish;
}

void TeleportCommand::Exit()
{
	Command::Exit();
}

void TeleportCommand::Broadcast()
{
	Proto::SyncPlayerCmd proto;
	proto.set_type((int)CmdType::Teleport);
	proto.set_player_sn(_owner->GetPlayerSN());
	proto.set_target_sn(target_sn);
	_owner->curWorld->BroadcastPacket(Proto::MsgId::S2C_SyncPlayerCmd, proto);
}

void TeleportCommand::Singlecast(Player* player)
{
	Proto::SyncPlayerCmd proto;
	proto.set_type((int)CmdType::Teleport);
	proto.set_player_sn(_owner->GetPlayerSN());
	proto.set_target_sn(target_sn);
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncPlayerCmd, proto, player);
}
