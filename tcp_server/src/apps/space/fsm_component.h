#ifndef FSM_COMPONENT
#define FSM_COMPONENT
#include "libserver/component.h"
#include "libserver/system.h"
#include "libserver/vector3.h"
#include "libplayer/player.h"
#include "ai_enemy.h"
#include "fsm_state.h"

class Player;
class AIEnemy;
class FsmState;
class FsmComponent :public Component<FsmComponent>, public IAwakeFromPoolSystem<>
{
	timeutil::Time _lastTime;
	AIEnemy* _parent = nullptr;
	FsmState* _prevState = nullptr;
	FsmState* _currState = nullptr;

public:
	~FsmComponent();

	void Awake() override;

	void BackToPool() override;

	void Update(AIEnemy* pEnemy);

	void ResetState();

	FsmState* GetCurrState() { return _currState; }

	void ChangeState(FsmState* newState);

	void SyncState(Proto::FsmSyncState& proto, Player* pPlayer);
};

#endif // !FSM_COMPONENT


