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
}

void PvpCommand::Singlecast(Player* player)
{
}
