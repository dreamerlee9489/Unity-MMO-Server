#include "cmd_move.h"

void MoveCommand::Enter()
{
	Command::Enter();
}

CmdState MoveCommand::Execute()
{
	if (_point.GetDistance(_owner->lastMap->GetCur()->Position) < 1)
		state = CmdState::Finish;
	return state;
}

void MoveCommand::Exit()
{
	Command::Exit();
}

void MoveCommand::Broadcast()
{
	Proto::SyncPlayerCmd proto;
	proto.set_type((int)CmdType::Move);
	proto.set_player_sn(_owner->GetPlayerSN());
	proto.set_target_sn(0);
	_point.SerializeToProto(proto.mutable_point());
	_owner->curWorld->BroadcastPacket(Proto::MsgId::S2C_SyncPlayerCmd, proto);
}

void MoveCommand::Singlecast(Player* player)
{
	Proto::SyncPlayerCmd proto;
	proto.set_type((int)CmdType::Move);
	proto.set_player_sn(_owner->GetPlayerSN());
	proto.set_target_sn(0);
	_point.SerializeToProto(proto.mutable_point());
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncPlayerCmd, proto, player);
}
