#include "fsm_component.h"
#include "fsm_patrol.h"
#include "fsm_idle.h"

FsmComponent::~FsmComponent()
{
	delete _currState;
	_currState = nullptr;
}

void FsmComponent::Awake()
{
	_parent = GetParent<Npc>();
	_currState = new Idle(_parent);
	AddTimer(1, 2, false, 0, BindFunP0(this, &FsmComponent::Start));
}

void FsmComponent::Start()
{
	_started = true;
	_currState->Enter();
}

void FsmComponent::BackToPool()
{
}

void FsmComponent::Update()
{
	if (_started && _currState)
		_currState->Execute();
}

void FsmComponent::ResetState()
{
	if (_currState->GetStateType() != FsmStateType::Patrol)
		ChangeState(new Idle((Npc*)GetParent()));
}

void FsmComponent::ChangeState(FsmState* newState)
{
	if (_prevState)
		delete _prevState;
	_prevState = _currState;
	_currState->Exit();
	_currState = newState;
	_currState->Enter();
}

void FsmComponent::SyncState(Proto::SyncFsmState& proto, Player* pPlayer)
{
	FsmStateType type = (FsmStateType)proto.state();
	if (!_currState->GetTarget() && type != _currState->GetStateType())
		ChangeState(FsmState::GenFsmState(type, _parent, pPlayer));
	else if (pPlayer == _currState->GetTarget())
		if (type != _currState->GetStateType())
			ChangeState(FsmState::GenFsmState(type, _parent, pPlayer));
}
