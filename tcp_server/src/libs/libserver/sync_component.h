#pragma once
#include "common.h"
#include "app_type.h"
#include "entity.h"

class Packet;

struct AppInfo
{
	APP_TYPE AppType;
	int AppId;
	std::string Ip;
	int Port;
	int Online;
	SOCKET Socket;

	bool Parse(Net::AppInfoSync proto);
};

class SyncComponent
{
public:
	void AppInfoSyncHandle(Packet* pPacket);
	void CmdShow();

protected:
	/// <summary>
	/// 找到在线人数最少的Appinfo
	/// </summary>
	/// <param name="appType"></param>
	/// <param name="pInfo"></param>
	/// <returns></returns>
	bool GetOneApp(APP_TYPE appType, AppInfo* pInfo);
	void Parse(Net::AppInfoSync proto, SOCKET socket);

	void HandleCmdApp(Packet* pPacket);
	virtual void HandleNetworkDisconnect(Packet* pPacket);

protected:
	// <appId, AppInfo>
	std::map<int, AppInfo> _apps;
};

