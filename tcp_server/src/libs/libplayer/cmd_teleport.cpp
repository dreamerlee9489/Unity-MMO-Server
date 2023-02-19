#include "cmd_teleport.h"

void TeleportCommand::Enter()
{
	Command::Enter();
}

CmdState TeleportCommand::Execute()
{
	return state;
}

void TeleportCommand::Exit()
{
	Command::Exit();
}

void TeleportCommand::Broadcast()
{
}

void TeleportCommand::Singlecast(Player* player)
{
}
