#pragma once

#include <map>
#include "common.h"
#include "entity.h"
#include "cache_swap.h"
#include "network_help.h"
#include "connect_obj.h"
#include "trace_component.h"

#if ENGINE_PLATFORM != PLATFORM_WIN32

#define MAX_CLIENT  5120

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

#ifdef EPOLL
#include <sys/epoll.h>
#endif

#define _sock_init( )
#define _sock_nonblock( sockfd ) { int flags = fcntl(sockfd, F_GETFL, 0); fcntl(sockfd, F_SETFL, flags | O_NONBLOCK); }
#define _sock_exit( )
#define _sock_err( )	errno
#define _sock_close( sockfd ) ::close( sockfd ) 
//#define _sock_close( sockfd ) ::shutdown(sockfd, SHUT_RDWR);
#define _sock_is_blocked()	(errno == EAGAIN || errno == 0)

#define RemoveConnectObj(socket) \
    _connects[socket]->ComponentBackToPool( ); \
    _connects[socket] = nullptr; \
    DeleteEvent(_epfd, socket); \
    _sockets.erase(socket); 

#else

#define MAX_CLIENT  10000

#define _sock_init( )	{ WSADATA wsaData; WSAStartup( MAKEWORD(2, 2), &wsaData ); }
#define _sock_nonblock( sockfd )	{ unsigned long param = 1; ioctlsocket(sockfd, FIONBIO, (unsigned long *)&param); }
#define _sock_exit( )	{ WSACleanup(); }
#define _sock_err( )	WSAGetLastError()
#define _sock_close( sockfd ) ::closesocket( sockfd )
#define _sock_is_blocked()	(WSAGetLastError() == WSAEWOULDBLOCK)

#define RemoveConnectObj(socket) \
    _connects[socket]->ComponentBackToPool( ); \
    _connects[socket] = nullptr; \
    _sockets.erase(socket); 

#define RemoveConnectObjByItem(iter) \
    _connects[*iter]->ComponentBackToPool(); \
    _connects[*iter] = nullptr; \
    iter = _sockets.erase(iter);

#endif

#if ENGINE_PLATFORM != PLATFORM_WIN32
#define SetsockOptType void *
#else
#define SetsockOptType const char *
#endif

class Packet;

class Network : public Entity<Network>, public INetwork
#if LOG_TRACE_COMPONENT_OPEN
	, public CheckTimeComponent
#endif
{
public:
	void BackToPool() override;
	void SendPacket(Packet*& pPacket) override;
	NetworkType GetNetworkType() const { return _networkType; }

protected:
	void SetSocketOpt(SOCKET socket);
	SOCKET CreateSocket();
	bool CheckSocket(SOCKET socket);
	bool CreateConnectObj(SOCKET socket, TagType tagType, TagValue tagValue, ConnectStateType iState);

	// packet
	void HandleDisconnect(Packet* pPacket);

#ifdef EPOLL
	void InitEpoll();
	void Epoll();
	/// <summary>
	/// 将Socket及其event添加到epollfd
	/// 创建Socket时，注册一次即可
	/// </summary>
	void AddEvent(int epollfd, int fd, int flag);
	/// <summary>
	/// 修改epollfd中已有的Socket及其event
	/// </summary>
	void ModifyEvent(int epollfd, int fd, int flag);
	/// <summary>
	/// 删除epollfd中的fd及其event
	/// </summary>
	void DeleteEvent(int epollfd, int fd);
	/// <summary>
	/// 对于NetworkListen来说，如果主Socket有连接事件发生，需要记录其下标
	/// </summary>
	virtual void OnEpoll(SOCKET fd, int index) { };
#else    
	void Select();
#endif

	void OnNetworkUpdate();

protected:
	ConnectObj* _connects[MAX_CLIENT]{};
	std::set<SOCKET> _sockets;

#ifdef EPOLL
#define MAX_EVENT   5120
	struct epoll_event _events[MAX_EVENT];
	int _epfd{ -1 };
#else
	SOCKET _fdMax{ INVALID_SOCKET };
	fd_set readfds, writefds, exceptfds;
#endif

	// 发送协议
	std::mutex _sendMsgMutex;
	CacheSwap<Packet> _sendMsgList;

	NetworkType _networkType{ NetworkType::TcpListen };
};
