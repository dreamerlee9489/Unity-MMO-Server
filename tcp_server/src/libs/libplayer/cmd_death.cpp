#include "cmd_death.h"

void DeathCommand::Enter()
{
	Command::Enter();
}

CmdState DeathCommand::Execute()
{
	return state = CmdState::Finish;
}

void DeathCommand::Exit()
{
	Command::Exit();
}

void DeathCommand::Broadcast()
{
	Proto::SyncPlayerCmd proto;
	proto.set_type((int)CmdType::Death);
	proto.set_player_sn(_owner->GetPlayerSN());
	proto.set_target_sn(target_sn);
	_owner->curWorld->BroadcastPacket(Proto::MsgId::S2C_SyncPlayerCmd, proto);
}

void DeathCommand::Singlecast(Player* player)
{
	Proto::SyncPlayerCmd proto;
	proto.set_type((int)CmdType::Death);
	proto.set_player_sn(_owner->GetPlayerSN());
	proto.set_target_sn(target_sn);
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncPlayerCmd, proto, player);
}
