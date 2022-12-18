#include "network_locator.h"
#include "network_listen.h"
#include <algorithm>
#include <utility>
#include "message_system_help.h"
#include "component_help.h"
#include "socket_object.h"
#include "global.h"
#include "message_system.h"

void NetworkLocator::Awake()
{
    std::lock_guard<std::mutex> g1(_lock);

    _netIdentify.clear();
    _connectors.clear();
    _listens.clear();

    // message
    auto pMsgSystem = GetSystemManager()->GetMessageSystem();

    // http
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_AppRegister, BindFunP1(this, &NetworkLocator::HandleAppRegister));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_NetworkDisconnect, BindFunP1(this, &NetworkLocator::HandleNetworkDisconnect));
}

void NetworkLocator::BackToPool()
{
    std::lock_guard<std::mutex> g1(_lock);

    _netIdentify.clear();
    _connectors.clear();
    _listens.clear();
}

void NetworkLocator::AddConnectorLocator(INetwork* pNetwork, NetworkType networkType)
{
    std::lock_guard<std::mutex> guard(_lock);
    _connectors[networkType] = pNetwork;
}

void NetworkLocator::AddNetworkIdentify(SocketKey* pSocket, uint64 appKey)
{
    _lock.lock();
    const auto iter = _netIdentify.find(appKey);
    if (iter != _netIdentify.end())
    {
        LOG_WARN("connector loacator recv multiple socket.");
        _netIdentify.erase(appKey);
    }

    NetIdentify netIdentify;
    netIdentify.GetSocketKey()->CopyFrom(pSocket);
    netIdentify.GetTagKey()->AddTag(TagType::App, appKey);
    _netIdentify.insert(std::make_pair(appKey, netIdentify));
    _lock.unlock();

    auto appType = GetTypeFromAppKey(appKey);
    auto appId = GetIdFromAppKey(appKey);
    LOG_DEBUG("connected appType:" << GetAppName(appType) << " appId:" << GetIdFromAppKey(appKey) << " " << &netIdentify);

    const auto pGlobal = Global::GetInstance();
    if ((pGlobal->GetCurAppType() & APP_APPMGR) == 0)
    {
        if (((appType & APP_APPMGR) != 0) || ((appType & APP_SPACE) != 0) || ((appType & APP_GAME) != 0))
        {
            // ����һ��ע��Э��
            Proto::AppRegister proto;
            proto.set_type(pGlobal->GetCurAppType());
            proto.set_id(pGlobal->GetCurAppId());
            MessageSystemHelp::SendPacket(Proto::MsgId::MI_AppRegister, proto, appType, appId);
        }
    }
}

std::list<NetIdentify> NetworkLocator::GetAppNetworks(const APP_TYPE appType)
{
    std::lock_guard<std::mutex> guard(_lock);
    std::list<NetIdentify> rs;

    auto iter = _netIdentify.begin();
    while (iter != _netIdentify.end()) {
        iter = std::find_if(iter, _netIdentify.end(), [appType](auto pair)
            {
                auto appKey = pair.first;
                return GetTypeFromAppKey(appKey) == appType;
            });

        if (iter != _netIdentify.end())
        {
            rs.emplace_back(iter->second);
            ++iter;
        }
    }

    return rs;
}

NetIdentify NetworkLocator::GetNetworkIdentify(const APP_TYPE appType, const int appId)
{
    std::lock_guard<std::mutex> guard(_lock);

    const auto appKey = GetAppKey(appType, appId);
    const auto iter = _netIdentify.find(appKey);
    if (iter == _netIdentify.end())
        return NetIdentify();

    return iter->second;
}

void NetworkLocator::AddListenLocator(INetwork* pNetwork, NetworkType networkType)
{
    std::lock_guard<std::mutex> guard(_lock);
    _listens[networkType] = pNetwork;
}

INetwork* NetworkLocator::GetNetwork(NetworkType networkType)
{
    std::lock_guard<std::mutex> guard(_lock);

    if (networkType == NetworkType::HttpListen || networkType == NetworkType::TcpListen)
    {
        auto iter = _listens.find(networkType);
        if (iter == _listens.end())
            return nullptr;

        return iter->second;
    }

    if (networkType == NetworkType::HttpConnector || networkType == NetworkType::TcpConnector)
    {
        auto iter = _connectors.find(networkType);
        if (iter == _connectors.end())
            return nullptr;

        return iter->second;
    }

    return nullptr;
}

void NetworkLocator::HandleAppRegister(Packet* pPacket)
{
    std::lock_guard<std::mutex> guard(_lock);

    const auto proto = pPacket->ParseToProto<Proto::AppRegister>();
    const uint64 appKey = GetAppKey(proto.type(), proto.id());

    const auto iter = _netIdentify.find(appKey);
    if (iter == _netIdentify.end())
    {
        NetIdentify netIdentify;
        netIdentify.GetSocketKey()->CopyFrom(pPacket->GetSocketKey());
        netIdentify.GetTagKey()->AddTag(TagType::App, appKey);
        _netIdentify.insert(std::make_pair(appKey, netIdentify));

        LOG_DEBUG("connected appType:" << GetAppName(GetTypeFromAppKey(appKey)) << " appId:" << GetIdFromAppKey(appKey) << " " << &netIdentify);

        // �޸�����ײ�ı�ʶ
        MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_NetworkListenKey, &netIdentify);
    } 
    else
    {
        _netIdentify[appKey].GetTagKey()->CopyFrom(pPacket->GetTagKey());
    }
}

void NetworkLocator::HandleNetworkDisconnect(Packet* pPacket)
{
    std::lock_guard<std::mutex> guard(_lock);

    auto pTags = pPacket->GetTagKey();
    auto pTagApp = pTags->GetTagValue(TagType::App);
    if (pTagApp == nullptr)
        return;

    auto appKey = pTagApp->KeyInt64;
    _netIdentify.erase(appKey);

    LOG_DEBUG("remove appType:" << GetAppName(GetTypeFromAppKey(appKey)) << " appId:" << GetIdFromAppKey(appKey));

    // �����ٴ�����
    auto appType = GetTypeFromAppKey(appKey);
    auto appId = GetIdFromAppKey(appKey);
    const auto pYaml = ComponentHelp::GetYaml();
    const auto pCommonConfig = pYaml->GetIPEndPoint(appType, appId);

    TagValue tagValue{ "", appKey };
    MessageSystemHelp::CreateConnect(NetworkType::TcpConnector, TagType::App, tagValue, pCommonConfig->Ip, pCommonConfig->Port);

}