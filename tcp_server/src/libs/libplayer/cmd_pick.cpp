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
}

void PickCommand::Singlecast(Player* player)
{
}
