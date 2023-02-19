#ifndef TELEPORT_COMMAND_H
#define TELEPORT_COMMAND_H
#include "command.h"

class TeleportCommand : public Command
{
	Vector3 _point{ 0, 0, 0 };

public:
	TeleportCommand(Player* owner, Vector3& point) : Command(owner), _point(point) {}

	void Enter() override;

	CmdState Execute() override;

	void Exit() override;

	void Broadcast() override;

	void Singlecast(Player* player) override;
};

#endif // !TELEPORT_COMMAND_H
