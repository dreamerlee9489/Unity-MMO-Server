#pragma once

#include <vector>
#include <map>
#include <string>
#include "libserver/vector3.h"

/// <summary>
/// 解析一行数据并初始化资源对象数据
/// </summary>
class ResourceBase
{
public:
	virtual ~ResourceBase() = default;
	explicit ResourceBase(std::map<std::string, int>& head) : _id(0), _head(head) {}

	int GetId() const { return _id; }
	/// <summary>
	/// 解析一行数据并将其转换为一个资源对象
	/// </summary>
	/// <param name="line"></param>
	/// <returns></returns>
	bool LoadProperty(const std::string line);
	// 有效性检查
	virtual bool Check() = 0;
	/// <summary>
	/// 解析一行并将其数据填充到_props
	/// </summary>
	/// <param name="line"></param>
	/// <returns></returns>
	static std::vector<std::string> ParserLine(std::string line);

protected:
	/// <summary>
	/// 初始化资源对象数据
	/// </summary>
	virtual void GenStruct() = 0;
	/// <summary>
	/// 根据字段名将数据解析为字符串
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	std::string GetString(std::string name);
	/// <summary>
	/// 根据字段名将数据解析为布尔值
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	bool GetBool(std::string name);
	/// <summary>
	/// 根据字段名将数据解析为整型
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	int GetInt(std::string name);
	/// <summary>
	/// 将string解析为Vector3
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	Vector3 GetVector(std::string& line);
	void DebugHead() const;

private:
	int _id;
	/// <summary>
	/// 字段名与索引下标的映射关系
	/// </summary>
	std::map<std::string, int>& _head;
	/// <summary>
	/// 储存字段数据(字符串格式)
	/// </summary>
	std::vector<std::string> _props;
};

