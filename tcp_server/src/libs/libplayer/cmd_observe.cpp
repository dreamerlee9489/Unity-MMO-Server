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
}

void ObserveCommand::Singlecast(Player* player)
{
}
