#pragma once

#include "libserver/log4_help.h"
#include "libserver/res_path.h"
#include "libserver/global.h"
#include "resource_base.h"

/// <summary>
/// 资源管理器基类
/// </summary>
/// <typeparam name="T">资源类型</typeparam>
template<class T>
class ResourceManagerTemplate {
public:
	virtual ~ResourceManagerTemplate() {
		_refs.clear();
	}
	/// <summary>
	/// 读取配置文件，初始化资源管理器数据
	/// </summary>
	/// <param name="table"></param>
	/// <param name="pResPath"></param>
	/// <returns></returns>
	bool Initialize(std::string table, ResPath* pResPath);
	/// <summary>
	/// 初始化之后的操作
	/// </summary>
	/// <returns></returns>
	virtual bool AfterInit() { return true; }
	/// <summary>
	/// 根据资源id获取一个资源实例
	/// </summary>
	/// <param name="id"></param>
	/// <returns></returns>
	T* GetResource(int id);

protected:
	/// <summary>
	/// 分析文件头
	/// </summary>
	/// <param name="line"></param>
	/// <returns></returns>
	bool ParserHead(std::string line);
	/// <summary>
	/// 分析文件行, 实例化一个资源对象并插入_refs
	/// </summary>
	/// <param name="line"></param>
	/// <returns></returns>
	bool LoadReference(std::string line);

protected:
	std::string _cvsName;
	std::map<std::string, int> _head;
	/// <summary>
	/// 资源id-资源实例映射
	/// </summary>
	std::map<int, T*> _refs;
};

template <class T>
bool ResourceManagerTemplate<T>::Initialize(std::string table, ResPath* pResPath)
{
	_cvsName = table;
	std::string path = pResPath->FindResPath("/resource");
	path = strutil::format("%s/%s.csv", path.c_str(), table.c_str());
	std::ifstream reader(path.c_str(), std::ios::in);
	if (!reader)
	{
		LOG_ERROR("can't open file. " << path.c_str());
		return false;
	}

	LOG_DEBUG("load file. " << path.c_str());

	if (reader.eof())
	{
		LOG_ERROR("read head failed. stream is eof.");
		return false;
	}
	// 分析标题行
	std::string line;
	std::getline(reader, line);
	std::transform(line.begin(), line.end(), line.begin(), ::tolower);

	if (!ParserHead(line))
	{
		LOG_ERROR("parse head failed. " << path.c_str());
		return false;
	}
	// 将每一行解析为一个资源对象, 存入_refs
	while (true)
	{
		if (reader.eof())
			break;

		std::getline(reader, line);

		if (line.empty())
			continue;

		std::transform(line.begin(), line.end(), line.begin(), ::tolower);

		LoadReference(line);
	}

	if (!AfterInit())
		return false;

	return true;
}

template <class T>
T* ResourceManagerTemplate<T>::GetResource(int id)
{
	auto iter = _refs.find(id);
	if (iter == _refs.end())
		return nullptr;

	return _refs[id];
}

template <class T>
bool ResourceManagerTemplate<T>::ParserHead(std::string line)
{
	if (line.empty())
		return false;

	std::vector<std::string> propertyList = ResourceBase::ParserLine(line);

	for (size_t i = 0; i < propertyList.size(); i++)
	{
		_head.insert(std::make_pair(propertyList[i], i));
	}

	return true;
}

/// <summary>
/// 解析一行数据并将资源ID-资源对象插入_refs
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="line"></param>
/// <returns></returns>
template <class T>
bool ResourceManagerTemplate<T>::LoadReference(std::string line)
{
	auto pT = new T(_head);
	if (pT->LoadProperty(line) && pT->Check())
	{
		_refs.insert(std::make_pair(pT->GetId(), pT));
		return true;
	}

	return false;
}
