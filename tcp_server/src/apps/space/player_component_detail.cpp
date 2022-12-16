#include "player_component_detail.h"

#include "libplayer/player.h"
#include "libserver/message_system_help.h"

void PlayerComponentDetail::Awake()
{
	Player* pPlayer = dynamic_cast<Player*>(_parent);
	ParserFromProto(pPlayer->GetPlayerProto());
}

void PlayerComponentDetail::BackToPool()
{
}

void PlayerComponentDetail::ParserFromProto(const Net::Player& proto)
{
	auto protoBase = proto.base();
	_gender = protoBase.gender();
}

void PlayerComponentDetail::SerializeToProto(Net::Player* pProto)
{

}

Net::Gender PlayerComponentDetail::GetGender() const
{
	return _gender;
}
