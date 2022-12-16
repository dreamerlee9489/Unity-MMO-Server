#include "player_component_onlineinlogin.h"
#include "libserver/redis_constants.h"

#include "libplayer/player.h"
#include "libserver/message_system_help.h"

void PlayerComponentOnlineInLogin::Awake(std::string account)
{
	_account = account;
	AddTimer(0, (int)(RedisKeyAccountOnlineLoginTimeout*0.5), true, 0, BindFunP0(this, &PlayerComponentOnlineInLogin::SetOnlineFlag));
}

void PlayerComponentOnlineInLogin::BackToPool()
{
	Net::AccountDeleteOnlineToRedis protoSync;
	protoSync.set_account(_account.c_str());
	MessageSystemHelp::DispatchPacket(Net::MsgId::MI_AccountDeleteOnlineToRedis, protoSync, nullptr);
}

void PlayerComponentOnlineInLogin::SetOnlineFlag() const
{
	// �������߱�־
	Net::AccountSyncOnlineToRedis protoSync;
	protoSync.set_account(_account.c_str());
	MessageSystemHelp::DispatchPacket(Net::MsgId::MI_AccountSyncOnlineToRedis, protoSync, nullptr);
}
