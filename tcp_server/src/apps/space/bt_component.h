#ifndef BT_COMPONENT
#define BT_COMPONENT
#include "libserver/component.h"
#include "libserver/system.h"
#include "libserver/vector3.h"
#include "libplayer/player.h"
#include "bt_node.h"
#include <queue>

class Player;
class Npc;
class BtNode;
struct BtEvent;
class BtComponent : public Component<BtComponent>, public IAwakeFromPoolSystem<>
{
public:
	std::priority_queue<BtEvent> events;

	~BtComponent();

	void Awake() override;

	void BackToPool() override;

	void Update();

	Player* GetTarget() { return _target; }

private:
	Npc* _npc = nullptr;
	Player* _target = nullptr;
	BtNode* _root = nullptr;
	
	void Start();
};

#endif // !BT_COMPONENT

