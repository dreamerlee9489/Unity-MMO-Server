#include "entity.h"
#include "entity_system.h"

void IEntity::ComponentBackToPool()
{
    auto pSystemManager = GetSystemManager();
    for (auto pair : _components)
    {
        // �� EntitySystem ȥ����
        if (pSystemManager != nullptr)
            pSystemManager->GetEntitySystem()->RemoveComponent(pair.second);
        else
            pair.second->ComponentBackToPool();
    }

    _components.clear();

    IComponent::ComponentBackToPool();
}

void IEntity::RemoveComponent(IComponent* pComponent)
{
    const auto typeHashCode = pComponent->GetTypeHashCode();
    _components.erase(typeHashCode);
    auto pSystemManager = GetSystemManager();
    if (pSystemManager == nullptr)
    {
        pComponent->ComponentBackToPool();
    }
    else
    {
        // �� EntitySystem ȥ����
        pSystemManager->GetEntitySystem()->RemoveComponent(pComponent);
    }
}
