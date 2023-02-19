#ifndef PICK_COMMAND_H
#define PICK_COMMAND_H
#include "command.h"

class PickCommand : public Command
{
	Vector3 _point{ 0, 0, 0 };

public:
	PickCommand(Player* owner, Vector3& point) : Command(owner), _point(point) {}

	void Enter() override;

	CmdState Execute() override;

	void Exit() override;

	void Broadcast() override;

	void Singlecast(Player* player) override;
};

#endif // !PICK_COMMAND_H
