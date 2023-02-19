#include "cmd_attack.h"

void AttackCommand::Enter()
{
    Command::Enter();
}

CmdState AttackCommand::Execute()
{
    if (_target->hp == 0)
        state = CmdState::Finish;
    return state;
}

void AttackCommand::Exit()
{
    Command::Exit();
}

void AttackCommand::Broadcast()
{
    Proto::SyncPlayerCmd proto;
    proto.set_type((int)CmdType::Attack);
    proto.set_player_sn(_owner->GetPlayerSN());
    proto.set_target_sn(_target->GetSN());
    _owner->curWorld->BroadcastPacket(Proto::MsgId::S2C_SyncPlayerCmd, proto);
}

void AttackCommand::Singlecast(Player* player)
{
    Proto::SyncPlayerCmd proto;
    proto.set_type((int)CmdType::Attack);
    proto.set_player_sn(_owner->GetPlayerSN());
    proto.set_target_sn(_target->GetSN());
    MessageSystemHelp::SendPacket(Proto::MsgId::S2C_SyncPlayerCmd, proto, player);
}
