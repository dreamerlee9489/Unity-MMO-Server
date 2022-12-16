#include "redis_login.h"
#include "libserver/protobuf/proto_id.pb.h"
#include "libserver/redis_constants.h"
#include "libserver/log4_help.h"
#include "libserver/thread_mgr.h"

#include "libserver/message_system_help.h"
#include "libserver/global.h"
#include "libserver/message_system.h"

void RedisLogin::RegisterMsgFunction()
{
    auto pMsgSystem = GetSystemManager()->GetMessageSystem();

    pMsgSystem->RegisterFunction(this, Net::MsgId::MI_AccountSyncOnlineToRedis, BindFunP1(this, &RedisLogin::HandleAccountSyncOnlineToRedis));
    pMsgSystem->RegisterFunction(this, Net::MsgId::MI_AccountDeleteOnlineToRedis, BindFunP1(this, &RedisLogin::HandleAccountDeleteOnlineToRedis));

    pMsgSystem->RegisterFunction(this, Net::MsgId::MI_LoginTokenToRedis, BindFunP1(this, &RedisLogin::HandleLoginTokenToRedis));
    pMsgSystem->RegisterFunction(this, Net::MsgId::MI_AccountQueryOnlineToRedis, BindFunP1(this, &RedisLogin::HandleAccountQueryOnline));
}


void RedisLogin::HandleAccountSyncOnlineToRedis(Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Net::AccountSyncOnlineToRedis>();

    const std::string key = RedisKeyAccountOnlineLogin + proto.account();
    const std::string value = std::to_string(Global::GetInstance()->GetCurAppId());
    Setex(key, value, RedisKeyAccountOnlineLoginTimeout);
}

void RedisLogin::HandleAccountDeleteOnlineToRedis(Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Net::AccountDeleteOnlineToRedis>();
    const std::string key = RedisKeyAccountOnlineLogin + proto.account();

    Delete(key);
}

void RedisLogin::HandleLoginTokenToRedis(Packet* pPacket)
{
    auto protoToken = pPacket->ParseToProto<Net::LoginTokenToRedis>();
    auto account = protoToken.account();
    auto playerSn = protoToken.player_sn();

    auto token = Global::GetInstance()->GenerateUUID();

    Net::TokenInfo protoInfo;
    protoInfo.set_token(token);
    protoInfo.set_player_sn(playerSn);

    std::string tokenString;
    protoInfo.SerializeToString(&tokenString);

    const std::string key = RedisKeyAccountTokey + account;
    const int timeoue = RedisKeyAccountTokeyTimeout;

    if (!Setex(key, tokenString, timeoue))
    {
        token = "";
        LOG_ERROR("account:" << account.c_str() << ". failed to set token.");
    }
    else
    {
#if LOG_REDIS_OPEN
        LOG_REDIS("account:" << account.c_str() << ". gen token:" << token.c_str());
#endif
    }

    Net::LoginTokenToRedisRs protoRs;
    protoRs.set_account(account.c_str());
    protoRs.set_token(token.c_str());
    MessageSystemHelp::DispatchPacket(Net::MsgId::MI_LoginTokenToRedisRs, protoRs, nullptr);
}

void RedisLogin::HandleAccountQueryOnline(Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Net::AccountQueryOnlineToRedis>();
    auto account = proto.account();
    Net::AccountQueryOnlineToRedisRs protoRs;
    protoRs.set_account(account.c_str());
    protoRs.set_return_code(Net::AccountQueryOnlineToRedisRs::SOTR_Offline);

    // 是否正在登录
    if (!SetnxExpire(RedisKeyAccountOnlineLogin + proto.account(), Global::GetInstance()->GetCurAppId(), RedisKeyAccountOnlineLoginTimeout))
        protoRs.set_return_code(Net::AccountQueryOnlineToRedisRs::SOTR_Online);

    // 是否 Game 在线
    if (GetInt(RedisKeyAccountOnlineGame + proto.account()) != 0)
        protoRs.set_return_code(Net::AccountQueryOnlineToRedisRs::SOTR_Online);


    MessageSystemHelp::DispatchPacket(Net::MsgId::MI_AccountQueryOnlineToRedisRs, protoRs, nullptr);
}
