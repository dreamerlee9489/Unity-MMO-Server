#ifndef CMD_COMPONENT_H
#define CMD_COMPONENT_H
#include "libserver/system.h"
#include "libserver/component.h"
#include "command.h"
#include "player.h"
#include <queue>

class Player;
class Command;
class CmdComponent : public Component<CmdComponent>, public IAwakeFromPoolSystem<>
{
	Player* _parent = nullptr;
	Command* _prevCmd = nullptr, * _currCmd = nullptr;
	
public:
	~CmdComponent() = default;

	void Awake() override;

	void BackToPool() override;

	Command* GetCurrCmd() { return _currCmd; }

	void Update();

	void ResetCmd();

	void ChangeCmd(Command* newCmd);
};

#endif // !CMD_COMPONENT_H
