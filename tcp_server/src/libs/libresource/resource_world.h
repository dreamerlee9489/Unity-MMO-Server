#pragma once

#include <string>
#include <vector>
#include "resource_base.h"
#include "resource_mgr_template.h"
#include "libserver/vector3.h"

struct ResourceEnemy
{
	int id = 0;
	std::string name = "";
	int level = 0;
	int initHp = 0;
	int initAtk = 0;
	Vector3 initPos{ 0, 0, 0 };
};

enum class ResourceWorldType
{
	Login = 1,
	Roles = 2, // 角色选择场景
	Public = 3,
	Dungeon = 4,
};

/// <summary>
/// 每一个地图场景对应一个ResourceWorld
/// </summary>
class ResourceWorld : public ResourceBase
{
public:
	explicit ResourceWorld(std::map<std::string, int>& head);
	bool Check() override;

	std::string GetName() const;
	ResourceWorldType GetType() const;
	bool IsType(ResourceWorldType iType) const;
	bool IsInitMap() const;
	Vector3 GetInitPosition() const;
	std::vector<ResourceEnemy> GetEnemies() const;

protected:
	/// <summary>
	/// 初始化地图资源数据
	/// </summary>
	void GenStruct() override;
	/// <summary>
	/// 解析敌人表标题行
	/// </summary>
	/// <param name="line"></param>
	/// <returns></returns>
	bool ParseEnemyCSV();

private:
	/// <summary>
	/// 地图名
	/// </summary>
	std::string _name{ "" };
	/// <summary>
	/// 敌人配置表路径
	/// </summary>
	std::string _enemyPath{ "" };
	/// <summary>
	/// 是否是初始地图
	/// </summary>
	bool _isInit{ false };
	/// <summary>
	/// 地图类型
	/// </summary>
	ResourceWorldType _worldType{ ResourceWorldType::Dungeon };
	/// <summary>
	/// 出生点
	/// </summary>
	Vector3 _initPosition{ 0,0,0 };
	std::vector<ResourceEnemy> _enemies;
};

class ResourceWorldMgr :public ResourceManagerTemplate<ResourceWorld>
{
public:
	bool AfterInit() override;
	/// <summary>
	/// 获取初始地图
	/// </summary>
	/// <returns></returns>
	ResourceWorld* GetInitMap();
	/// <summary>
	/// 获取选角场景
	/// </summary>
	/// <returns></returns>
	ResourceWorld* GetRolesMap();

private:
	int _initMapId{ 0 };
	int _rolesMapId{ 0 };
};
