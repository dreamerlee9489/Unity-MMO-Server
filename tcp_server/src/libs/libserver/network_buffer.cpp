#include "packet.h"
#include "network_buffer.h"
#include "connect_obj.h"
#include "message_system_help.h"
#include "network.h"

#include <cstdlib>
#include <cstring>
#include "mongoose/mongoose.h"

NetworkBuffer::NetworkBuffer(const unsigned int size, ConnectObj* pConnectObj)
{
    _pConnectObj = pConnectObj;
    _bufferSize = size;
    _beginIndex = 0;
    _endIndex = 0;
    _dataSize = 0;
    _buffer = new char[_bufferSize];
}

NetworkBuffer::~NetworkBuffer()
{
    delete[] _buffer;
}

void NetworkBuffer::BackToPool()
{
    _beginIndex = 0;
    _endIndex = 0;
    _dataSize = 0;
}

bool NetworkBuffer::HasData() const
{
    if (_dataSize <= 0)
        return false;

    // ����Ҫ��һ��Э��ͷ
    if (_dataSize < sizeof(PacketHead))
        return false;

    return true;
}

unsigned int NetworkBuffer::GetEmptySize()
{
    return _bufferSize - _dataSize;
}

unsigned int NetworkBuffer::GetWriteSize() const
{
    if (_beginIndex <= _endIndex)
    {
        return _bufferSize - _endIndex;
    }
    else
    {
        return _beginIndex - _endIndex;
    }
}

unsigned int NetworkBuffer::GetReadSize() const
{
    if (_dataSize <= 0)
        return 0;

    if (_beginIndex < _endIndex)
    {
        return _endIndex - _beginIndex;
    }
    else
    {
        return _bufferSize - _beginIndex;
    }
}

void NetworkBuffer::FillDate(unsigned int  size)
{
    _dataSize += size;

    // �ƶ���ͷ��
    if ((_bufferSize - _endIndex) <= size)
    {
        size -= _bufferSize - _endIndex;
        _endIndex = 0;
    }

    _endIndex += size;
}

void NetworkBuffer::RemoveDate(unsigned int size)
{
    _dataSize -= size;

    if ((_beginIndex + size) >= _bufferSize)
    {
        size -= _bufferSize - _beginIndex;
        _beginIndex = 0;
    }

    _beginIndex += size;
}

void NetworkBuffer::ReAllocBuffer()
{
    Buffer::ReAllocBuffer(_dataSize);
}


///////////////////////////////////////////////////////////////////////////////////////////////

RecvNetworkBuffer::RecvNetworkBuffer(const unsigned int size, ConnectObj* pConnectObj) : NetworkBuffer(size, pConnectObj) {

}

int RecvNetworkBuffer::GetBuffer(char*& pBuffer) const
{
    pBuffer = _buffer + _endIndex;
    return GetWriteSize();
}

Packet* RecvNetworkBuffer::GetPacket()
{
    auto pNetwork = _pConnectObj->GetParent<Network>();
    if (!NetworkHelp::IsTcp(pNetwork->GetNetworkType()))
        return GetHttpPacket();

    return GetTcpPacket();
}

Packet* RecvNetworkBuffer::GetTcpPacket()
{
    // ���ݳ��Ȳ���
    if (_dataSize < sizeof(TotalSizeType))
    {
        return nullptr;
    }

    // 1.���� ���峤��
    unsigned short totalSize;
    MemcpyFromBuffer(reinterpret_cast<char*>(&totalSize), sizeof(TotalSizeType));

    // Э���峤�Ȳ������ȴ�
    if (_dataSize < totalSize)
    {
        return nullptr;
    }

    RemoveDate(sizeof(TotalSizeType));

    // 2.ͷ����
    unsigned short headSize;
    MemcpyFromBuffer(reinterpret_cast<char*>(&headSize), sizeof(TotalSizeType));
    RemoveDate(sizeof(TotalSizeType));

    // 3.���� PacketHead
    PacketHead* pHead;
    PacketHeadS2S* pHeadS2s = nullptr;
    if (headSize == sizeof(PacketHead)) 
    {
        PacketHead head;
        MemcpyFromBuffer(reinterpret_cast<char*>(&head), sizeof(PacketHead));
        RemoveDate(sizeof(PacketHead));
        pHead = &head;
    }
    else
    {
        PacketHeadS2S head;
        MemcpyFromBuffer(reinterpret_cast<char*>(&head), sizeof(PacketHeadS2S));
        RemoveDate(sizeof(PacketHeadS2S));
        pHead = &head;
        pHeadS2s = &head;
    }

    // 4.���� Э��
    // ���һ��id
    const google::protobuf::EnumDescriptor* descriptor = Proto::MsgId_descriptor();
    if (descriptor->FindValueByNumber(pHead->MsgId) == nullptr)
    {
        // �ر�����
        _pConnectObj->Close();
        std::cout << "recv invalid msg." << std::endl;
        return nullptr;
    }

    Packet* pPacket = MessageSystemHelp::CreatePacket((Proto::MsgId)pHead->MsgId, _pConnectObj);
    unsigned int dataLength = totalSize - sizeof(PacketHead) - sizeof(TotalSizeType) * 2;
    if (pHeadS2s != nullptr)
        dataLength = totalSize - sizeof(PacketHeadS2S) - sizeof(TotalSizeType) * 2;

    while (pPacket->GetTotalSize() < dataLength)
    {
        pPacket->ReAllocBuffer();
    }

    MemcpyFromBuffer(pPacket->GetBuffer(), dataLength);
    pPacket->FillData(dataLength);
    RemoveDate(dataLength);

    if (pHeadS2s != nullptr)
    {
        auto pTagKey = pPacket->GetTagKey();
        pTagKey->AddTag(TagType::Entity, pHeadS2s->EntitySn);
        pTagKey->AddTag(TagType::Player, pHeadS2s->PlayerSn);
    }

    return pPacket;
}

Packet* RecvNetworkBuffer::GetHttpPacket()
{
    if (_endIndex < _beginIndex)
    {
        // ���쳣���ر�����
        _pConnectObj->Close();
        LOG_ERROR("http recv invalid.");
        return nullptr;
    }

    const unsigned int recvBufLength = _endIndex - _beginIndex;
    const auto pNetwork = _pConnectObj->GetParent<Network>();
    const auto iType = pNetwork->GetNetworkType();
    const bool isConnector = iType == NetworkType::HttpConnector;

    http_message hm;
    const unsigned int headerLen = mg_parse_http(_buffer + _beginIndex, _endIndex - _beginIndex, &hm, !isConnector);
    if (headerLen <= 0)
        return nullptr;

    unsigned int bodyLen = 0;
    const auto mgBody = mg_get_http_header(&hm, "Content-Length");
    if (mgBody != nullptr)
    {
        bodyLen = atoi(mgBody->p);

        // �������ĳ��Ȳ������ٵ�һ��
        if (bodyLen > 0 && (recvBufLength < (bodyLen + headerLen)))
            return nullptr;
    }

    bool isChunked = false;
    const auto mgTransferEncoding = mg_get_http_header(&hm, "Transfer-Encoding");
    if (mgTransferEncoding != nullptr && mg_vcasecmp(mgTransferEncoding, "chunked") == 0)
    {
        isChunked = true;

        // ��������ݻ�û�е���
        if (recvBufLength == headerLen)
            return nullptr;

        bodyLen = mg_http_get_request_len(_buffer + _beginIndex + headerLen, recvBufLength - headerLen);
        if (bodyLen <= 0)
            return nullptr;

        bodyLen = _endIndex - _beginIndex - headerLen;
    }

#ifdef LOG_HTTP_OPEN
    // ��ӡһ�����ݿ���
    std::stringstream allBuffer;
    allBuffer.write(_buffer + _beginIndex, (bodyLen + headerLen));
    LOG_HTTP("\r\n" << allBuffer.str().c_str());
#endif

    Packet* pPacket = MessageSystemHelp::ParseHttp(_pConnectObj, _buffer + _beginIndex + headerLen, bodyLen, isChunked, &hm);
    RemoveDate(bodyLen + headerLen);
    return pPacket;
}

void RecvNetworkBuffer::MemcpyFromBuffer(char* pVoid, const unsigned int size)
{
    const auto readSize = GetReadSize();
    if (readSize < size)
    {
        // 1.copyβ������
        ::memcpy(pVoid, _buffer + _beginIndex, readSize);

        // 2.copyͷ������
        ::memcpy(pVoid + readSize, _buffer, size - readSize);
    }
    else
    {
        ::memcpy(pVoid, _buffer + _beginIndex, size);
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////

SendNetworkBuffer::SendNetworkBuffer(const unsigned int size, ConnectObj* pConnectObj) : NetworkBuffer(size, pConnectObj)
{

}

int SendNetworkBuffer::GetBuffer(char*& pBuffer) const
{
    if (_dataSize <= 0)
    {
        pBuffer = nullptr;
        return 0;
    }

    if (_beginIndex < _endIndex)
    {
        pBuffer = _buffer + _beginIndex;
        return _endIndex - _beginIndex;
    }
    else
    {
        pBuffer = _buffer + _beginIndex;
        return _bufferSize - _beginIndex;
    }
}

void SendNetworkBuffer::AddPacket(Packet* pPacket)
{
    const auto dataLength = pPacket->GetDataLength();
    const auto pTagValue = pPacket->GetTagKey()->GetTagValue(TagType::Entity);

    // ���һ�������ܵĳ��ȣ�Head�������
    // ��ʽΪ���ܳ� + ͷ���� + ͷ�� + Э����
    TotalSizeType totalSize = dataLength + sizeof(PacketHead) + sizeof(TotalSizeType) * 2;
    if (pTagValue != nullptr)
    {
        totalSize = dataLength + sizeof(PacketHeadS2S) + sizeof(TotalSizeType) * 2;
    }

    // ���Ȳ���������
    while (GetEmptySize() < totalSize) {
        ReAllocBuffer();
        //std::cout << "send buffer::Realloc. _bufferSize:" << _bufferSize << std::endl;
    }

    // ����httpЭ����˵��ֻ��body��û���Զ���ͷ
    const auto msgId = pPacket->GetMsgId();
    if (!NetworkHelp::IsHttpMsg(msgId))
    {
        // 1.���峤��
        MemcpyToBuffer(reinterpret_cast<char*>(&totalSize), sizeof(TotalSizeType));

        // 2.ͷ��
        if (pTagValue == nullptr)
        {
            PacketHead head{};
            head.MsgId = pPacket->GetMsgId();

            TotalSizeType headSize = sizeof(PacketHead);
            MemcpyToBuffer(reinterpret_cast<char*>(&headSize), sizeof(TotalSizeType));
            MemcpyToBuffer(reinterpret_cast<char*>(&head), sizeof(PacketHead));
        }
        else
        {
            PacketHeadS2S head{};
            head.MsgId = pPacket->GetMsgId();

            head.EntitySn = pTagValue->KeyInt64;
            const auto pTagPlayer = pPacket->GetTagKey()->GetTagValue(TagType::Player);
            if (pTagPlayer == nullptr)
                head.PlayerSn = 0;
            else
                head.PlayerSn = pTagPlayer->KeyInt64;

            TotalSizeType headSize = sizeof(PacketHeadS2S);
            MemcpyToBuffer(reinterpret_cast<char*>(&headSize), sizeof(TotalSizeType));
            MemcpyToBuffer(reinterpret_cast<char*>(&head), sizeof(PacketHeadS2S));
        }
    }

    // 3.����
    MemcpyToBuffer(pPacket->GetBuffer(), pPacket->GetDataLength());
}

void SendNetworkBuffer::MemcpyToBuffer(char* pVoid, const unsigned int size)
{
    const auto writeSize = GetWriteSize();
    if (writeSize < size)
    {
        // 1.copy��β��
        ::memcpy(_buffer + _endIndex, pVoid, writeSize);

        // 2.copy��ͷ��
        ::memcpy(_buffer, pVoid + writeSize, size - writeSize);
    }
    else
    {
        ::memcpy(_buffer + _endIndex, pVoid, size);
    }

    FillDate(size);
}
