#include "message_system.h"
#include <utility>
#include "system_manager.h"
#include "packet.h"
#include "entity_system.h"
#include "component.h"
#include "object_pool_packet.h"
#include "component_help.h"

MessageSystem::MessageSystem(SystemManager* pMgr)
{
    _systemMgr = pMgr;
}

void MessageSystem::Dispose()
{

}

void MessageSystem::AddPacketToList(Packet* pPacket)
{
    std::lock_guard<std::mutex> guard(_packet_lock);
    _cachePackets.GetWriterCache()->emplace_back(pPacket);

    // ����ʱ Ref +1
    pPacket->AddRef();
}

void MessageSystem::RegisterFunction(IEntity* obj, int msgId, MsgCallbackFun cbfun)
{
    auto iter = _callbacks.find(msgId);
    if (iter == _callbacks.end())
    {
        _callbacks.insert(std::make_pair(msgId, new std::map<uint64, IMessageCallBack*>()));
    }

    // ���� MessageCallBack
    const auto pCallback = _systemMgr->GetEntitySystem()->AddComponent<MessageCallBack>(std::move(cbfun));
    pCallback->SetParent(obj);

    // �������SN������keyֵ�У����ڲ���
    _callbacks[msgId]->insert(std::make_pair(obj->GetSN(), pCallback));
}

void MessageSystem::RegisterDefaultFunction(IEntity* obj, MsgCallbackFun cbfun)
{
    // ���� MessageCallBack
    const auto pCallback = _systemMgr->GetEntitySystem()->AddComponent<MessageCallBack>(std::move(cbfun));
    pCallback->SetParent(obj);

    _defaultCallbacks.insert(std::make_pair(obj->GetSN(), pCallback));
}

void MessageSystem::RemoveFunction(IComponent* obj)
{
    for (auto iter1 = _callbacks.begin(); iter1 != _callbacks.end(); ++iter1)
    {
        auto pSub = iter1->second;
        const auto iter2 = pSub->find(obj->GetSN());
        if (iter2 == pSub->end())
            continue;

        pSub->erase(iter2);
    }

    _defaultCallbacks.erase(obj->GetSN());
}

void MessageSystem::Update(EntitySystem* pEntities)
{
    _packet_lock.lock();
    if (_cachePackets.CanSwap())
    {
        _cachePackets.Swap();
    }
    _packet_lock.unlock();

    if (_cachePackets.GetReaderCache()->empty())
        return;

    auto packetLists = _cachePackets.GetReaderCache();
    for (auto iter = packetLists->begin(); iter != packetLists->end(); ++iter)
    {
        auto pPacket = (*iter);
        uint64 entitySn = 0;
        auto pTags = pPacket->GetTagKey();
        const auto pTagValue = pTags->GetTagValue(TagType::Entity);
        if (pTagValue != nullptr)
        {
            entitySn = pTagValue->KeyInt64;
        }

        const auto msgIterator = _callbacks.find(pPacket->GetMsgId());
        bool isDo = false;

        if (msgIterator != _callbacks.end())
        {
            auto pSub = msgIterator->second;
            if (entitySn > 0)
            {
                const auto iterSub = pSub->find(entitySn);
                if (iterSub != pSub->end())
                {
                    if (iterSub->second->ProcessPacket(pPacket))
                        isDo = true;
                }
            }
            else
            {
                for (auto iterSub = pSub->begin(); iterSub != pSub->end(); ++iterSub)
                {
                    if (iterSub->second->ProcessPacket(pPacket))
                        isDo = true;
                }
            }
        }

        if (!isDo)
        {
            // �Ƿ���Ĭ�ϴ�����
            if (entitySn > 0)
            {
                // allinone ʱ ��world��worldproxy sn��ͬ������world��Э�飬�ǲ�������Ĭ�ϴ�������
                const auto pTagToWorld = pTags->GetTagValue(TagType::ToWorld);
                if (pTagToWorld == nullptr)
                {
                    auto pMsgCallback = _defaultCallbacks[entitySn];
                    if (pMsgCallback != nullptr)
                    {
                        pMsgCallback->ProcessPacket(pPacket);
                    }
                }
            }
        }

        // �뿪ʱ Ref - 1
        pPacket->RemoveRef();
    }

    _cachePackets.GetReaderCache()->clear();
}
