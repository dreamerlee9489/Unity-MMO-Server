#include "move_system.h"
#include "move_component.h"
#include "libserver/entity_system.h"
#include "libplayer/player.h"
#include "libplayer/player_component_last_map.h"

MoveSystem::MoveSystem()
{
    _lastTime = Global::GetInstance()->TimeTick;
}

void MoveSystem::Update(EntitySystem* pEntities)
{
    _currTime = Global::GetInstance()->TimeTick;
    _timeElapsed = _currTime - _lastTime;
    if (_timeElapsed < 100)
        return;

    if (_pCollections == nullptr)
    {
        _pCollections = pEntities->GetComponentCollections<MoveComponent>();
        if (_pCollections == nullptr)
            return;
    }

    _lastTime = _currTime;
    const auto& plists = *_pCollections->GetAll();
    for (auto iter = plists.begin(); iter != plists.end(); ++iter)
    {
        auto pMoveComponent = dynamic_cast<MoveComponent*>(iter->second);
        auto pNpc = pMoveComponent->GetParent<Npc>();
        if (pNpc)
            pMoveComponent->Update(_timeElapsed, pNpc, pMoveComponent->moveSpeed);
        else
        {
            auto pPlayer = pMoveComponent->GetParent<Player>();
            if (pMoveComponent->Update(_timeElapsed, pPlayer->GetComponent<PlayerComponentLastMap>(), 5.56f))
                pPlayer->RemoveComponent<MoveComponent>();
        }
    }
}
