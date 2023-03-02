#ifndef BT_COMPONENT
#define BT_COMPONENT
#include "libserver/component.h"
#include "libserver/system.h"
#include "libserver/vector3.h"
#include "libplayer/player.h"
#include "bt_action.h"
#include "bt_composite.h"
#include <queue>

class Player;
class Npc;
class BtNode;
struct BtEvent;
class BtComponent : public Component<BtComponent>, public IAwakeFromPoolSystem<>
{
public:	
	BtAction* curAct = nullptr;

	~BtComponent();

	void Awake() override;

	void BackToPool() override;

	void Update();

	void AddEvent(BtEventId id, int priority = 0) { _events.emplace(id, priority); }

	void SyncAction(Player* pPlayer);

	Player* GetTarget() { return _target; }

private:
	Npc* _npc = nullptr;
	Player* _target = nullptr;
	BtNode* _root = nullptr;
	std::priority_queue<BtEvent> _events;

	void ParseConfig(BtConfig* pConfig);
};

#endif // !BT_COMPONENT

