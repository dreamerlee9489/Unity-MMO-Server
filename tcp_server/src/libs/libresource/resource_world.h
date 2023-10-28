#pragma once

#include <string>
#include <vector>
#include "resource_base.h"
#include "resource_mgr_template.h"
#include "libserver/vector3.h"

struct ResourceNpc
{
	int id = 0;
	int level = 0;
	int initHp = 0;
	int initAtk = 0;
	std::string name = "";
	Vector3 initPos{ 0, 0, 0 };
};

enum struct ResourceWorldType
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
	~ResourceWorld();
	bool Check() override { return true; }

	std::string GetName() const;
	ResourceWorldType GetType() const;
	bool IsType(ResourceWorldType iType) const;
	bool IsInitMap() const;
	Vector3 GetInitPosition() const;
	std::vector<ResourceNpc>* GetNpcCfgs() { return &_npcCfgs; }
	std::vector<int>* GetPotionCfgs() { return &_potionCfgs; }
	std::vector<int>* GetWeaponCfgs() { return &_weaponCfgs; }

protected:
	/// <summary>
	/// 初始化地图资源数据
	/// </summary>
	void GenStruct() override;
	/// <summary>
	/// 解析敌人表
	/// </summary>
	void ParseEnemyCSV();
	/// <summary>
	/// 解析道具表
	/// </summary>
	void ParseItemsCSV();

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
	std::vector<ResourceNpc> _npcCfgs;
	std::vector<int> _potionCfgs, _weaponCfgs;
};

class ResourceWorldMgr :public ResourceManagerTemplate<ResourceWorld>
{
public:
	bool AfterInit() override;
	ResourceWorld* GetInitMap();
	ResourceWorld* GetRolesMap();

private:
	int _initMapId{ -2 };
	int _rolesMapId{ -1 };
};
