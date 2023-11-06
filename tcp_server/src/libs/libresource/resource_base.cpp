﻿#include "resource_base.h"
#include "libserver/log4_help.h"
#include "libserver/util_string.h"

std::vector<std::string> ResourceBase::ParserLine(std::string line)
{
	line = strutil::trim(line);

	std::vector<std::string> propertyList;
	bool isQuoted = false;
	int index = 0;

	do
	{
		//LOG_DEBUG( "size:" << line.length( ) << "\tline:" << line.c_str( ) );

		if (line.at(0) == '\"')
			isQuoted = true;
		else
			isQuoted = false;

		if (isQuoted)
		{
			line.erase(0, 1);
			index = line.find('\"');
		}
		else
		{
			index = line.find(',');
		}

		if (index > 0)
		{
			//LOG_DEBUG( "\tpro:" << line.substr( 0, index ).c_str( ) );
			propertyList.push_back(line.substr(0, index));
			line = line.erase(0, index + 1);
		}
		else
		{
			//LOG_DEBUG( "size:" << line.length( ) << "\tpro:" << line.c_str( ) );
			propertyList.push_back(line);
			break;
		}

		if (line.empty())
			break;

	} while (true);

	return propertyList;
}

/// <summary>
/// 解析一行数据并初始化资源类型数据
/// </summary>
/// <param name="line"></param>
/// <returns></returns>
bool ResourceBase::LoadProperty(const std::string line)
{
	std::vector<std::string> propertyList = ParserLine(line);

	// csv 行最后可能一个看不见的字符，所以propertyList一定是大于_head的
	if (propertyList.size() < _head.size())
	{
		LOG_ERROR("LoadProperty failed. " << "line size:" << propertyList.size() << " head size:" << _head.size() << " \t" << line.c_str());
		return false;
	}

	for (size_t i = 0; i < propertyList.size(); i++)
	{
		_props.push_back(strutil::trim(propertyList[i]));
	}

	_id = std::stoi(_props[0]);

	GenStruct();
	return true;
}

Vector3 ResourceBase::GetVector(std::string& line)
{
	Vector3 vec{ 0, 0, 0 };
	line = line.substr(1, line.size() - 2);
	std::vector<std::string> res;
	size_t begin = 0, end = line.size() - 1;
	while ((end = line.find(';')) != std::string::npos) {
		res.push_back(line.substr(begin, end - begin));
		line = line.substr(end + 1);
	}
	res.push_back(line);
	vec.X = std::stof(res[0]);
	vec.Y = std::stof(res[1]);
	vec.Z = std::stof(res[2]);
	return vec;
}

void ResourceBase::DebugHead() const
{
	for (auto one : _head)
	{
		LOG_DEBUG("head name:[" << one.first.c_str() << "] head index:" << one.second);
	}
}

bool ResourceBase::GetBool(std::string name)
{
	const auto iter = _head.find(name);
	if (iter == _head.end())
	{
		LOG_ERROR("GetInt Failed. id:" << _id << " name:[" << name.c_str() << "]");
		DebugHead();
		return false;
	}

	return std::stoi(_props[iter->second]) == 1;
}

int ResourceBase::GetInt(std::string name)
{
	const auto iter = _head.find(name);
	if (iter == _head.end())
	{
		LOG_ERROR("GetInt Failed. id:" << _id << " name:[" << name.c_str() << "]");
		DebugHead();
		return 0;
	}

	return std::stoi(_props[iter->second]);
}

std::string ResourceBase::GetString(std::string name)
{
	const auto iter = _head.find(name);
	if (iter == _head.end())
	{
		LOG_ERROR("GetString Failed. id:" << _id << " name:[" << name.c_str() << "]");
		DebugHead();
		return "";
	}

	return _props[iter->second];
}


