#include "bt_component.h"
#include "bt_act_birth.h"
#include "bt_act_death.h"
#include "bt_act_view.h"
#include "bt_act_sense.h"
#include "bt_act_idle.h"
#include "bt_act_patrol.h"
#include "bt_act_pursue.h"
#include "bt_condition.h"
#include "bt_repeat.h"
#include "bt_parallel.h"
#include "bt_selector.h"
#include "bt_sequence.h"

BtComponent::~BtComponent()
{
}

void BtComponent::Awake()
{
	_npc = GetParent<Npc>();
	BtActBirth* actBirth = new BtActBirth(_npc);
	BtActDeath* actDeath = new BtActDeath(_npc);
	BtParallel* parAlive = new BtParallel(_npc);
	BtSequence* seqLife = new BtSequence(_npc);
	BtActView* actView = new BtActView(_npc);
	BtActSense* actSense = new BtActSense(_npc);
	BtSelector* selBehav = new BtSelector(_npc);
	BtActIdle* actIdle = new BtActIdle(_npc);
	BtActPatrol* actPatrol = new BtActPatrol(_npc);
	BtActPursue* actPursue = new BtActPursue(_npc);
	seqLife->AddChild(actBirth);
	seqLife->AddChild(parAlive);
	seqLife->AddChild(actDeath);
	selBehav->AddChild(actIdle);
	selBehav->AddChild(actPatrol);
	selBehav->AddChild(actPursue);
	parAlive->AddChild(actView);
	parAlive->AddChild(actSense);
	parAlive->AddChild(selBehav);
	_root = new BtRepeat(_npc, seqLife);
	AddTimer(1, 2, false, 0, BindFunP0(this, &BtComponent::Start));
}

void BtComponent::BackToPool()
{
}

void BtComponent::Update()
{
	if (_root)
		_root->Tick();
	while (!events.empty())
	{
		const BtEvent& e = events.top();
		events.pop();
		_root->HandleEvent(e.id);
	}
}

void BtComponent::Start()
{
	events.push(BtEvent(BtEventId::Alive));
}
