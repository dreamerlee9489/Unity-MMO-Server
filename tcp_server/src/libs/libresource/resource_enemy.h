#pragma once

#include <string>

#include "resource_base.h"
#include "resource_mgr_template.h"
#include "libserver/vector3.h"

/// <summary>
/// 每一个地图场景对应一个ResourceEnemy
/// </summary>
class ResourceEnemy : public ResourceBase
{
public:
	explicit ResourceEnemy(std::map<std::string, int>& head) : ResourceBase(head)
	{
	}

	bool Check() override
	{
	}

protected:
	/// <summary>
	/// 初始化地图资源数据
	/// </summary>
	void GenStruct() override
	{

	}

private:
	/// <summary>
	/// 地图名
	/// </summary>
	std::string _name{ "" };
	/// <summary>
	/// 是否是初始地图
	/// </summary>
	bool _isInit{ false };
	/// <summary>
	/// 地图类型
	/// </summary>
	/// <summary>
	/// 出生点
	/// </summary>
};

class ResourceEnemyMgr :public ResourceManagerTemplate<ResourceEnemy>
{
public:
	bool AfterInit() override
	{
	}

private:
};
