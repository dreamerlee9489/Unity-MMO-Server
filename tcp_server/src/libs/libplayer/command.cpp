#include "command.h"
#include "cmd_move.h"
#include "cmd_attack.h"
#include "cmd_pick.h"
#include "cmd_teleport.h"
#include "cmd_observe.h"
#include "cmd_pvp.h"

Command* Command::GenCmd(CmdType type, Player* owner, Vector3& point, IEntity* target)
{
	switch (type)
	{
	case CmdType::Move:
		return new MoveCommand(owner, point);
	case CmdType::Attack:
		return new AttackCommand(owner, target);
	case CmdType::Pick:
		return new PickCommand(owner, point);
	case CmdType::Teleport:
		return new TeleportCommand(owner, point);
	case CmdType::Observe:
		return new ObserveCommand(owner, target);
	case CmdType::Pvp:
		return new PvpCommand(owner, target);
	default:
		return nullptr;
	}
}
