#include "resource_world.h"
#include "libserver/log4_help.h"
#include <libserver/component_help.h>

ResourceWorld::ResourceWorld(std::map<std::string, int>& head) : ResourceBase(head)
{
}

ResourceWorld::~ResourceWorld()
{
	delete& _enemies;
	delete& _potions;
	delete& _weapons;
}

std::string ResourceWorld::GetName() const
{
	return _name;
}

ResourceWorldType ResourceWorld::GetType() const
{
	return _worldType;
}


bool ResourceWorld::IsInitMap() const
{
	return _isInit;
}

bool ResourceWorld::IsType(ResourceWorldType iType) const
{
	return _worldType == iType;
}

Vector3 ResourceWorld::GetInitPosition() const
{
	return _initPosition;
}

void ResourceWorld::GenStruct()
{
	_name = GetString("name");
	_isInit = GetBool("init");
	_enemyPath = GetString("enemycsv");
	_worldType = static_cast<ResourceWorldType>(GetInt("type"));
	std::string value = GetString("playerinitpos");
	std::vector<std::string> params;
	strutil::split(value, ',', params);
	if (params.size() == 3)
	{
		_initPosition.X = std::stof(params[0]);
		_initPosition.Y = std::stof(params[1]);
		_initPosition.Z = std::stof(params[2]);
	}
	if (_enemyPath != "null")
		ParseEnemyCSV();
	ParseItemsCSV();
}

void ResourceWorld::ParseEnemyCSV()
{
	//LOG_DEBUG("ResourceWorld::GenStruct _enemyPath=" << _enemyPath.c_str());
	const auto pResPath = ComponentHelp::GetResPath();
	std::string path = pResPath->FindResPath("/resource");
	path = strutil::format("%s/%s.csv", path.c_str(), _enemyPath.c_str());
	std::ifstream reader(path.c_str(), std::ios::in);
	if (!reader)
	{
		LOG_ERROR("cant open file. " << path.c_str());
		return;
	}
	if (reader.eof())
	{
		LOG_ERROR("read head failed. stream is eof.");
		return;
	}

	std::string line;
	std::getline(reader, line);
	while (!reader.eof())
	{
		std::getline(reader, line);
		if (!line.empty())
		{
			std::vector<std::string> properties = ParserLine(line);
			ResourceEnemy enemy;
			enemy.id = std::stoi(properties[0]);
			enemy.name = properties[1];
			enemy.level = std::stoi(properties[2]);
			enemy.initHp = std::stoi(properties[3]);
			enemy.initAtk = std::stoi(properties[4]);
			enemy.initPos = GetVector(properties[5]);
			_enemies.push_back(enemy);
		}
	}
}

void ResourceWorld::ParseItemsCSV()
{
	const auto pResPath = ComponentHelp::GetResPath();
	std::string path = pResPath->FindResPath("/resource");
	std::string line;
	path = strutil::format("%s/ItemPotions.csv", path.c_str());
	std::ifstream reader(path.c_str(), std::ios::in);
	if (!reader || reader.eof())
	{
		LOG_ERROR("cant open file. " << path.c_str());
		return;
	}
	std::getline(reader, line);
	while (!reader.eof())
	{
		std::getline(reader, line);
		if (!line.empty())
		{
			std::vector<std::string> properties = ParserLine(line);
			_potions.push_back(std::stoi(properties[0]));
		}
	}

	path = pResPath->FindResPath("/resource");
	path = strutil::format("%s/ItemWeapons.csv", path.c_str());
	reader = std::ifstream(path.c_str(), std::ios::in);
	if (!reader || reader.eof())
	{
		LOG_ERROR("cant open file. " << path.c_str());
		return;
	}
	std::getline(reader, line);
	while (!reader.eof())
	{
		std::getline(reader, line);
		if (!line.empty())
		{
			std::vector<std::string> properties = ParserLine(line);
			_weapons.push_back(std::stoi(properties[0]));
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// 遍历_refs，初始化_initMapId、_rolesMapId
/// </summary>
/// <returns></returns>
bool ResourceWorldMgr::AfterInit()
{
	auto iter = _refs.begin();
	while (iter != _refs.end())
	{
		auto pRef = iter->second;

		if (pRef->IsInitMap())
		{
			if (_initMapId != 0)
			{
				LOG_ERROR("map has tow init Map. id1:" << _initMapId << " id2:" << pRef->GetId());
			}
			_initMapId = pRef->GetId();
		}

		if (pRef->IsType(ResourceWorldType::Roles))
		{
			_rolesMapId = pRef->GetId();
		}

		++iter;
	}

	return true;
}

ResourceWorld* ResourceWorldMgr::GetInitMap()
{
	if (_initMapId > 0)
		return GetResource(_initMapId);

	return nullptr;
}

ResourceWorld* ResourceWorldMgr::GetRolesMap()
{
	if (_rolesMapId > 0)
		return GetResource(_rolesMapId);

	return nullptr;
}
