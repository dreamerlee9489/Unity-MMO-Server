#include "bt_component.h"

void BtComponent::Awake()
{
	_npc = GetParent<Npc>();
	ParseConfig(ComponentHelp::GetYaml()->GetConfig(_npc->GetWorld()->IsPublic() ? NPC_TYPE::Public : NPC_TYPE::Dungeon));
}

void BtComponent::Update()
{
	if (_root)
		_root->Tick();
	while (!_events.empty())
	{
		const BtEvent& e = _events.top();
		_events.pop();
		_root->HandleEvent(e.id);
	}
}

void BtComponent::AddEvent(BtEventId id, int priority)
{
	_events.emplace(id, priority);
}

void BtComponent::SyncAction(Player* pPlayer)
{
	if (curAct)
		curAct->Singlecast(pPlayer);
}

void BtComponent::ParseConfig(BtConfig* pConfig)
{
	_npc->rebirth = pConfig->rebirth;
	BtActBirth* actBirth = new BtActBirth(_npc);
	BtActDeath* actDeath = new BtActDeath(_npc);
	BtParallel* parAlive = new BtParallel(_npc);
	BtSequence* seqLife = new BtSequence(_npc);
	BtActView* actView = new BtActView(_npc);
	BtActSense* actSense = new BtActSense(_npc);
	BtEdSelector* selBehav = new BtEdSelector(_npc);
	for (auto& one : pConfig->nodes)
	{
		if (one.type == "BtActPatrol")
			selBehav->AddChild(new BtActPatrol(_npc));
		else if (one.type == "BtActIdle")
			selBehav->AddChild(new BtActIdle(_npc));
		else if (one.type == "BtActPursue")
			selBehav->AddChild(new BtActPursue(_npc));
		else if (one.type == "BtActAttack")
			selBehav->AddChild(new BtActAttack(_npc));
		else if (one.type == "BtActFlee")
			selBehav->AddChild(new BtActFlee(_npc));
	}
	seqLife->AddChildren({ actBirth, parAlive, actDeath });
	parAlive->AddChildren({ actView, actSense, selBehav });
	_root = new BtRepeat(_npc, seqLife);
}