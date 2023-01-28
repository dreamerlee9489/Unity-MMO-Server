﻿#pragma once
#include "component.h"
#include "disposable.h"

#include <list>
#include <unordered_map>

// ComponentCollections 是按组件类型来存的
class ComponentCollections : public IDisposable
{
public:
	ComponentCollections(std::string componentName);
	~ComponentCollections();

	void Add(IComponent* pObj);
	void Remove(uint64 sn);

	IComponent* Get(uint64 sn = 0);
	std::unordered_map<uint64, IComponent*>* GetAll();

	void Swap();
	void Dispose() override;

	std::string GetClassType() const;

private:
	std::unordered_map<uint64, IComponent*> _objs;
	std::unordered_map<uint64, IComponent*> _addObjs;
	std::list<uint64> _removeObjs;

	std::string _componentName{ "" };
};
