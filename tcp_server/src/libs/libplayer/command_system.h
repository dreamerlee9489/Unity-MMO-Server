#ifndef CMD_SYSTEM_H
#define CMD_SYSTEM_H
#include "libserver/system.h"
#include "libserver/entity_system.h"
#include "libserver/util_time.h"
#include "command_component.h"

class CmdSystem : public ISystem<CmdSystem>
{
private:
	timeutil::Time _lastTime = 0, _currTime = 0, _timeElapsed = 0;
	ComponentCollections* _pCollections{ nullptr };

public:
	CmdSystem();

	void Update(EntitySystem* pEntities) override;
};

#endif // !CMD_SYSTEM_H
