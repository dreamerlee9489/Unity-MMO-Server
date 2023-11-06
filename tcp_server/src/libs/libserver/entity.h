#pragma once
#include <map>
#include <list>
#include <memory>
#include <algorithm>
#include <queue>
#include "component.h"
#include "entity_system.h"

class IEntity :public IComponent
{
public:
	virtual ~IEntity() = default;
	void ComponentBackToPool() override;

	template <class T, typename... TArgs>
	T* AddComponent(TArgs... args);

	template <class T, typename... TArgs>
	T* AddComponentWithSn(uint64 sn, TArgs... args);

	template<class T>
	T* GetComponent();

	template<class T>
	void RemoveComponent();

	void RemoveComponent(IComponent* pObj);

protected:
	// <type hash code , IComponent>
	std::map<uint64, IComponent*> _components;
};

template <class T, typename... TArgs>
inline T* IEntity::AddComponent(TArgs... args)
{
	return AddComponentWithSn<T>(0, std::forward<TArgs>(args)...);
}

template <class T, typename ... TArgs>
T* IEntity::AddComponentWithSn(uint64 sn, TArgs... args)
{
	const auto typeHashCode = typeid(T).hash_code();
	if (_components.find(typeHashCode) == _components.end())
	{
		T* pComponent = _pSystemManager->GetEntitySystem()->AddComponentWithParent<T>(this, sn, std::forward<TArgs>(args)...);
		_components.insert(std::make_pair(typeHashCode, pComponent));
		return pComponent;
	}
	return nullptr;
}

template<class T>
T* IEntity::GetComponent()
{
	const auto iter = _components.find(typeid(T).hash_code());
	if (iter != _components.end())
		return dynamic_cast<T*>(iter->second);
	return nullptr;
}

template<class T>
void IEntity::RemoveComponent()
{
	// 先删除本地组件数据
	const auto iter = _components.find(typeid(T).hash_code());
	if (iter != _components.end())
		RemoveComponent(iter->second);
}

template<class T>
class Entity : public IEntity
{
public:
	const char* GetTypeName() override;
	uint64 GetTypeHashCode() override;
};

template <class T>
const char* Entity<T>::GetTypeName()
{
	return typeid(T).name();
}

template <class T>
uint64 Entity<T>::GetTypeHashCode()
{
	return typeid(T).hash_code();
}
