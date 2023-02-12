﻿#include <iostream>
#include <set>

#include "common.h"
#include "network_listen.h"
#include "connect_obj.h"
#include "thread_mgr.h"
#include "network_locator.h"
#include "log4_help.h"
#include "component_help.h"
#include "global.h"
#include "message_system.h"
#include "update_component.h"

void NetworkListen::Awake(std::string ip, int port, NetworkType iType)
{
	_networkType = iType;

	// 
	auto pNetworkLocator = ThreadMgr::GetInstance()->GetEntitySystem()->GetComponent<NetworkLocator>();
	pNetworkLocator->AddListenLocator(this, iType);

	// message
	auto pMsgSystem = GetSystemManager()->GetMessageSystem();

	pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_NetworkRequestDisconnect, BindFunP1(this, &NetworkListen::HandleDisconnect));
	pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_NetworkListenKey, BindFunP1(this, &NetworkListen::HandleListenKey));

	// update
	AddComponent<UpdateComponent>(BindFunP0(this, &NetworkListen::Update));

	// 
	_masterSocket = CreateSocket();
	if (_masterSocket == INVALID_SOCKET)
		return;

	// 快速重启地址
	int isOn = 1;
	setsockopt(_masterSocket, SOL_SOCKET, SO_REUSEADDR, (SetsockOptType)&isOn, sizeof(isOn));

	// 
	sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	::inet_pton(AF_INET, ip.c_str(), &addr.sin_addr.s_addr);

	if (::bind(_masterSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
	{
		LOG_ERROR("::bind failed. err:" << _sock_err() << " ip=" << ip.c_str() << " port=" << addr.sin_port);
		return;
	}

	const int maxConn = 1024; // SOMAXCONN
	if (::listen(_masterSocket, maxConn) < 0)
	{
		std::cout << "::listen failed." << _sock_err() << " ip=" << ip.c_str() << " port=" << addr.sin_port << std::endl;
		return;
	}

#ifdef EPOLL
	LOG_INFO("epoll model. listen " << ip.c_str() << ":" << port << " SOMAXCONN:" << maxConn);
	InitEpoll();
	AddEvent(_epfd, _masterSocket, EPOLLIN | EPOLLET | EPOLLOUT | EPOLLRDHUP);
#else
	LOG_INFO("select model. listen " << ip.c_str() << ":" << port << " SOMAXCONN:" << maxConn);
#endif

	return;
}

void NetworkListen::BackToPool()
{
#ifdef EPOLL
	_mainSocketEventIndex = -1;
#endif

	//std::cout << "network dispose. close socket:" << _socket << std::endl;
	_sock_close(_masterSocket);
	_masterSocket = INVALID_SOCKET;

	Network::BackToPool();
}

void NetworkListen::Awake(int appType, int appId)
{
	auto pGlobal = Global::GetInstance();
	auto pYaml = ComponentHelp::GetYaml();
	const auto pCommonConfig = pYaml->GetIPEndPoint(pGlobal->GetCurAppType(), pGlobal->GetCurAppId());
	if (pCommonConfig == nullptr)
	{
		LOG_ERROR("failed to get config of listen. appType:" << GetAppName(pGlobal->GetCurAppType()) << " appId:" << pGlobal->GetCurAppId());
		return;
	}

	Awake(pCommonConfig->IntraIp, pCommonConfig->Port, NetworkType::TcpListen);
}

void NetworkListen::Awake(std::string ip, int port)
{
	Awake(ip, port, NetworkType::HttpListen);
}

int NetworkListen::Accept()
{
	struct sockaddr socketClient;
	socklen_t socketLength = sizeof(socketClient);

	int rs = 0;
	while (true)
	{
		const SOCKET socket = ::accept(_masterSocket, &socketClient, &socketLength);
		if (socket == INVALID_SOCKET)
			return rs;

		//LOG_DEBUG("accept socket:" << socket << " networktype:" << GetNetworkTypeName(_networkType));
		if (!CreateConnectObj(socket, TagType::None, TagValue{}, ConnectStateType::Connected))
		{
			_sock_close(socket);
			continue;
		}

		SetSocketOpt(socket);
		++rs;
	}
}

const char* NetworkListen::GetTypeName()
{
	return typeid(NetworkListen).name();
}

uint64 NetworkListen::GetTypeHashCode()
{
	return typeid(NetworkListen).hash_code();
}

void NetworkListen::CmdShow()
{
	LOG_DEBUG("\tsocket size:" << _sockets.size());
}

#ifndef EPOLL

void NetworkListen::Update()
{
#if LOG_TRACE_COMPONENT_OPEN
	CheckBegin();
#endif

	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);

	FD_SET(_masterSocket, &readfds);
	FD_SET(_masterSocket, &writefds);
	FD_SET(_masterSocket, &exceptfds);

	_fdMax = _masterSocket;

	Select();

#if LOG_TRACE_COMPONENT_OPEN
	CheckPoint(std::string(GetNetworkTypeName(_networkType)) + "01");
#endif

	if (FD_ISSET(_masterSocket, &readfds))
	{
		Accept();
	}

	Network::OnNetworkUpdate();

#if LOG_TRACE_COMPONENT_OPEN
	CheckPoint(std::string(GetNetworkTypeName(_networkType)) + "02");
#endif

}

#else

void NetworkListen::OnEpoll(SOCKET fd, int index)
{
	if (fd == _masterSocket)
	{
		_mainSocketEventIndex = index;
	}
}

void NetworkListen::Update()
{
	_mainSocketEventIndex = -1;

	Epoll();

	if (_mainSocketEventIndex >= 0)
		Accept();

	Network::OnNetworkUpdate();
}

#endif

void NetworkListen::HandleListenKey(Packet* pPacket)
{
	const auto socketKey = pPacket->GetSocketKey();
	if (socketKey->NetType != _networkType)
		return;

	const auto socket = socketKey->Socket;
	if (_sockets.find(socket) == _sockets.end())
	{
		std::cout << "failed to modify connect key. socket not find." << pPacket << std::endl;
		return;
	}

	auto pObj = _connects[socket];
	pObj->GetTagKey()->CopyFrom(pPacket->GetTagKey());
}
