#pragma once
#include "libserver/component.h"
#include "libserver/system.h"
#include "libserver/vector3.h"
#include "libplayer/player.h"
#include "bt_node.h"
#include <queue>

class Player;
class Npc;
class BtNode;
class BtAction;
struct BtEvent;
enum struct BtEventId;
class BtComponent : public Component<BtComponent>, public IAwakeFromPoolSystem<>
{
public:	
	BtAction* curAct = nullptr;

	~BtComponent() {}

	void Awake() override;

	void BackToPool() override {}

	void Update();

	void AddEvent(BtEventId id, int priority = 0);

	void SyncAction(Player* pPlayer);

	Player* GetTarget() { return _target; }

private:
	Npc* _npc = nullptr;
	Player* _target = nullptr;
	BtNode* _root = nullptr;
	std::priority_queue<BtEvent> _events;

	void ParseConfig(BtConfig* pConfig);
};