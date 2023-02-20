#include "cmd_observe.h"

void ObserveCommand::Enter()
{
	Command::Enter();
}

CmdState ObserveCommand::Execute()
{
	return state;
}

void ObserveCommand::Exit()
{
	Command::Exit();
}

void ObserveCommand::Broadcast()
{
	Proto::SyncPlayerCmd proto;
	proto.set_type((int)CmdType::Observe);
	proto.set_player_sn(_owner->GetPlayerSN());
	proto.set_target_sn(_target->GetPlayerSN());
	_owner->curWorld->BroadcastPacket(Proto::MsgId::S2C_SyncPlayerCmd, proto);
}

void ObserveCommand::Singlecast(Player* player)
{
	Proto::SyncPlayerCmd proto;
	proto.set_type((int)CmdType::Observe);
	proto.set_player_sn(_owner->GetPlayerSN());
	proto.set_target_sn(_target->GetPlayerSN());
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncPlayerCmd, proto, player);
}
