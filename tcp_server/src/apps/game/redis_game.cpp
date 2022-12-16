#include "redis_game.h"
#include "libserver/redis_constants.h"
#include "libserver/message_system_help.h"
#include "libserver/message_system.h"

void RedisGame::RegisterMsgFunction()
{
    auto pMsgSystem = GetSystemManager()->GetMessageSystem();

    pMsgSystem->RegisterFunction(this, Net::MsgId::MI_PlayerSyncOnlineToRedis, BindFunP1(this, &RedisGame::HandlePlayerSyncOnlineToRedis));
    pMsgSystem->RegisterFunction(this, Net::MsgId::MI_PlayerDeleteOnlineToRedis, BindFunP1(this, &RedisGame::HandlePlayerDeleteOnlineToRedis));
    pMsgSystem->RegisterFunction(this, Net::MsgId::MI_GameTokenToRedis, BindFunP1(this, &RedisGame::HandleGameTokenToRedis));
}

void RedisGame::HandlePlayerSyncOnlineToRedis(Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Net::PlayerSyncOnlineToRedis>();
    auto curValue = proto.version();

    const std::string key = RedisKeyAccountOnlineGame + proto.account();
    const auto onlineVersion = this->GetInt(key);

    if (curValue < onlineVersion)
        return;

    Setex(key, curValue, RedisKeyAccountOnlineGameTimeout);
}

void RedisGame::HandlePlayerDeleteOnlineToRedis(Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Net::PlayerDeleteOnlineToRedis>();
    auto curValue = proto.version();

    const std::string key = RedisKeyAccountOnlineGame + proto.account();
    const auto onlineVersion = this->GetInt(key);

    if (curValue < onlineVersion)
        return;

    Delete(key);
}

void RedisGame::HandleGameTokenToRedis(Packet* pPacket)
{
    auto protoToken = pPacket->ParseToProto<Net::GameTokenToRedis>();

    Net::GameTokenToRedisRs protoRs;
    protoRs.set_account(protoToken.account().c_str());

    const std::string tokenValue = GetString(RedisKeyAccountTokey + protoToken.account());
    protoRs.mutable_token_info()->ParseFromString(tokenValue);

    Delete(RedisKeyAccountTokey + protoToken.account());

    MessageSystemHelp::DispatchPacket(Net::MsgId::MI_GameTokenToRedisRs, protoRs, nullptr);
}
