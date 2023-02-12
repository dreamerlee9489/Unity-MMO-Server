#include "player_component_proto_list.h"

void PlayerComponentProtoList::BackToPool()
{
    for (auto iter = _protos.begin(); iter != _protos.end(); ++iter)
    {
        delete iter->second;
    }

    _protos.clear();
}

void PlayerComponentProtoList::Parse(Proto::PlayerList& proto)
{
    const int size = proto.players_size();
    for (int i = 0; i < size; i++)
    {
        auto playerSn = proto.players(i).sn();
        std::stringstream* pStream = new std::stringstream();
        proto.players(i).SerializeToOstream(pStream);
        _protos[playerSn] = pStream;
    }
}

std::stringstream* PlayerComponentProtoList::GetProto(const uint64 playerSn)
{
    const auto iter = _protos.find(playerSn);
    if (iter == _protos.end())
        return nullptr;

    return iter->second;
}

