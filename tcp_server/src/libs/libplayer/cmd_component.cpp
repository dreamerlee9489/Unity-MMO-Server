#include "cmd_component.h"
#include "cmd_none.h"

void CmdComponent::Awake()
{
	_parent = GetParent<Player>();
}

void CmdComponent::BackToPool()
{
}

void CmdComponent::Update()
{
	if (_currCmd && _currCmd->Execute() == CmdState::Finish)
		ChangeCmd(nullptr);
}

void CmdComponent::ResetCmd()
{
	ChangeCmd(new NoneCommand(_parent));
}

void CmdComponent::ChangeCmd(Command* newCmd)
{
	if (_prevCmd)
		delete _prevCmd;
	_prevCmd = _currCmd;
	if (_currCmd)
		_currCmd->Exit();
	_currCmd = newCmd;
	if (newCmd)
		_currCmd->Enter();
}
