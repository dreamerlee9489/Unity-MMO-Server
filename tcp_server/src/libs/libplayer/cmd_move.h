#ifndef MOVE_COMMAND_H
#define MOVE_COMMAND_H
#include "command.h"

class Command;
class Player;
class MoveCommand : public Command
{
	Vector3 _point{0, 0, 0};

public:
	MoveCommand(Player* owner, Vector3& point) : Command(owner), _point(point) {}

	void Enter() override;

	CmdState Execute() override;

	void Exit() override;

	void Broadcast() override;

	void Singlecast(Player* player) override;
};

#endif // !MOVE_COMMAND_H
