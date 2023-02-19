#include "cmd_death.h"

void DeathCommand::Enter()
{
	Command::Enter();
}

CmdState DeathCommand::Execute()
{
	return state;
}

void DeathCommand::Exit()
{
	Command::Exit();
}

void DeathCommand::Broadcast()
{
}

void DeathCommand::Singlecast(Player* player)
{
}
