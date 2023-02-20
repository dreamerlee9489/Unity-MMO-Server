#include "cmd_pick.h"

void PickCommand::Enter()
{
	Command::Enter();
}

CmdState PickCommand::Execute()
{
	return state;
}

void PickCommand::Exit()
{
	Command::Exit();
}

void PickCommand::Broadcast()
{
	Proto::SyncPlayerCmd proto;
	proto.set_type((int)CmdType::Pick);
	proto.set_player_sn(_owner->GetPlayerSN());
	proto.set_target_sn(target_sn);
	_point.SerializeToProto(proto.mutable_point());
	_owner->curWorld->BroadcastPacket(Proto::MsgId::S2C_SyncPlayerCmd, proto);
}

void PickCommand::Singlecast(Player* player)
{
	Proto::SyncPlayerCmd proto;
	proto.set_type((int)CmdType::Pick);
	proto.set_player_sn(_owner->GetPlayerSN());
	proto.set_target_sn(target_sn);
	_point.SerializeToProto(proto.mutable_point());
	MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncPlayerCmd, proto, player);
}
