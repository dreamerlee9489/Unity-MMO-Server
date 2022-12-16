#pragma once
#include "common.h"
#include "app_type.h"
#include "packet.h"
#include "network_type.h"

class INetwork;
struct http_message;

struct ParseUrlInfo
{
	std::string Host;
	int Port;
	std::string Mothed;
	std::string Params;
};

class MessageSystemHelp
{
public:
	static Packet* CreatePacket(Net::MsgId msgId, NetIdentify* pIdentify);

	static void CreateConnect(NetworkType iType, TagType tagType, TagValue& tagValue, std::string ip, int port);

	static void DispatchPacket(const Net::MsgId msgId, NetIdentify* pIdentify);
	static void DispatchPacket(const Net::MsgId msgId, google::protobuf::Message& proto, NetIdentify* pIdentify);

	static void SendPacket(const Net::MsgId msgId, google::protobuf::Message& proto, NetIdentify* pIdentify);
	static void SendPacket(const Net::MsgId msgId, google::protobuf::Message& proto, APP_TYPE appType, int appId = 0);
	static void SendPacket(const Net::MsgId msgId, google::protobuf::Message& proto, TagKey* pTagKey, APP_TYPE appType, int appId = 0);
	static void SendPacket(const Net::MsgId msgId, TagKey* pTagKey, APP_TYPE appType, int appId = 0);
	/// <summary>
	/// 调用网络层SendPacket
	/// </summary>
	/// <param name="pPacket"></param>
	static void SendPacket(Packet* pPacket);
	static void SendPacket(Packet* pPacket, APP_TYPE appType, int appId);

	static void SendPacketToAllApp(Net::MsgId msgId, google::protobuf::Message& proto, APP_TYPE appType);

	// http
	// 发送请求
	static void SendHttpRequest(NetIdentify* pIdentify, std::string ip, int port, std::string method, std::map<std::string, std::string>* pParams);

	// 发送返回
	static void SendHttpResponse(NetIdentify* pIdentify, const char* content, int size);
	static void SendHttpResponse404(NetIdentify* pIdentify);

	// 分析数据
	static Packet* ParseHttp(NetIdentify* pIdentify, const char* s, unsigned int bodyLen, const bool isChunked, http_message* hm);
	static bool ParseUrl(const std::string& url, ParseUrlInfo& info);

protected:
	static void DispatchPacket(Packet* packet);

	// http
	static void SendHttpResponseBase(NetIdentify* pIdentify, int status_code, const char* content, int size);
};

