#pragma once
#include "common.h"
#include "network_type.h"
#include <log4cplus/streams.h>
#include <ostream>

struct NetIdentify;

/// <summary>
/// 发送方的唯一网络标识
/// </summary>
struct SocketKey
{
	SocketKey(SOCKET socket, NetworkType netType);

	void Clear();
	void CopyFrom(SocketKey* pSocketKey);
	void CopyFrom(NetIdentify* pIdentify);

	SOCKET Socket;
	NetworkType NetType;

	bool operator != (const SocketKey other) const
	{
		if (Socket != other.Socket)
			return false;

		if (NetType != other.NetType)
			return false;

		return true;
	};

	bool operator == (const SocketKey other) const
	{
		return (Socket == other.Socket) && (NetType == other.NetType);
	};

	static SocketKey None;
};

/// <summary>
/// TagKey中保存的某一个键
/// </summary>
enum struct TagType
{
	None = Proto::TagType::TagTypeNone,
	Account = Proto::TagType::TagTypeAccount,
	App = Proto::TagType::TagTypeApp,
	Entity = Proto::TagType::TagTypeEntity,
	ToWorld = Proto::TagType::TagTypeToWorld,
	Player = Proto::TagType::TagTypePlayer,
	Socket = Proto::TagType::TagTypeSocket
};

inline bool IsTagTypeStr(const TagType iType)
{
	return iType == TagType::Account;
}

inline const char* GetTagTypeName(const TagType iType)
{
	if (iType == TagType::Account)
		return "Account";
	else if (iType == TagType::App)
		return "App";
	else if (iType == TagType::Entity)
		return "World";
	else if (iType == TagType::Player)
		return "Player";
	else if (iType == TagType::Socket)
		return "Socket";
	else
		return "None";
}

/// <summary>
/// TagKey中保存的某一个值
/// </summary>
struct TagValue
{
	/// <summary>
	/// 目标TagType
	/// </summary>
	std::string KeyStr{ "" };
	/// <summary>
	/// 目标序列号
	/// </summary>
	uint64 KeyInt64{ 0 };

	bool operator != (const TagValue& other) const
	{
		return (KeyStr != other.KeyStr) || (KeyInt64 != other.KeyInt64);
	};

	bool operator == (const TagValue& other) const
	{
		return (KeyStr == other.KeyStr) && (KeyInt64 == other.KeyInt64);
	};
};

/// <summary>
/// 接收方唯一标签集合
/// </summary>
struct TagKey
{
public:
	void Clear();

	std::map<TagType, TagValue>* GetTags() { return &_tags; }

	void AddTag(TagType tagType, std::string value);
	void AddTag(TagType tagType, uint64 value);
	void AddTag(TagType tagType, TagValue value);

	TagValue* GetTagValue(TagType tagType);
	void CopyFrom(TagKey* pNetIdentify);
	bool CompareTags(TagKey* pIdentify);

protected:
	static bool CompareTags(TagKey* pA, TagKey* pB, TagType tagtype);

protected:
	/// <summary>
	/// 多标签(目标标签类型-目标带标签序列号)
	/// </summary>
	std::map<TagType, TagValue> _tags;
};

struct NetIdentify
{
public:
	NetIdentify() = default;
	~NetIdentify()
	{
		_socketKey.Clear();
		_tagKey.Clear();
	}

	SocketKey* GetSocketKey() { return &_socketKey; }
	TagKey* GetTagKey() { return &_tagKey; }

protected:
	SocketKey _socketKey{ INVALID_SOCKET, NetworkType::None };
	TagKey _tagKey;
};

std::ostream& operator <<(std::ostream& os, TagKey* pTagKey);
std::ostream& operator <<(std::ostream& os, NetIdentify* pIdentify);

#if ENGINE_PLATFORM == PLATFORM_WIN32
log4cplus::tostream& operator <<(log4cplus::tostream& os, TagKey* pTagKey);
log4cplus::tostream& operator <<(log4cplus::tostream& os, NetIdentify* pIdentify);
#endif
