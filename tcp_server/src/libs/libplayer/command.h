#ifndef COMMAND_H
#define COMMAND_H
#include "player.h"

class Player;
class Npc;
enum struct CmdType { None, Move, Attack, Pick, Teleport, Observe, Pvp, Death };
enum struct CmdState { Invalid, Running, Finish };

class Command
{
public:
	CmdState state = CmdState::Invalid;
	uint64 target_sn = 0;

	Command(Player* owner) : _owner(owner) {}
	
	virtual void Enter() 
	{ 
		Broadcast();
		state = CmdState::Running; 
	}

	virtual CmdState Execute() { return state; }

	virtual void Exit() { state = CmdState::Finish; }

	virtual void Broadcast() = 0;

	virtual void Singlecast(Player* player) = 0;

protected:
	Player* _owner = nullptr;
};

#endif // !COMMAND_H
