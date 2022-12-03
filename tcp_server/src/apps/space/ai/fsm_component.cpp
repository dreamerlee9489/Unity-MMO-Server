﻿#include "./fsm_component.h"
#include "./patrol.h"
#include "./idle.h"

FsmComponent::~FsmComponent()
{
	delete _currState;
	_currState = nullptr;
}

void FsmComponent::Awake()
{
	_parent = (AIEnemy*)GetParent();
	_currState = new Idle(_parent);
	_lastTime = Global::GetInstance()->TimeTick;
}

void FsmComponent::BackToPool()
{
}

void FsmComponent::Update(AIEnemy* pEnemy)
{
	if (_currState != nullptr)
		_currState->Execute();
}

void FsmComponent::ResetState()
{
	ChangeState(new Idle((AIEnemy*)GetParent(), nullptr));
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

void FsmComponent::SyncState(Proto::FsmSyncState& proto, Player* pPlayer)
{
	FsmStateType type = (FsmStateType)proto.state();
	if (!_currState->GetTarget() && type != _currState->GetStateType())
	{
		Vector3 curPos, nxtPos;
		curPos.ParserFromProto(proto.curpos());
		nxtPos.ParserFromProto(proto.nxtpos());
		_parent->SetCurrPos(curPos);
		_parent->SetNextPos(nxtPos);
		ChangeState(FsmState::GenFsmState(type, _parent, pPlayer));
	}
	else if (pPlayer == _currState->GetTarget())
	{
		Vector3 curPos, nxtPos;
		curPos.ParserFromProto(proto.curpos());
		nxtPos.ParserFromProto(proto.nxtpos());
		_parent->SetCurrPos(curPos);
		_parent->SetNextPos(nxtPos);
		if (type != _currState->GetStateType())
			ChangeState(FsmState::GenFsmState(type, _parent, pPlayer));
	}
}
