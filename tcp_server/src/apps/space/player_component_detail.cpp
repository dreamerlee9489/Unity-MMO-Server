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

void PlayerComponentDetail::ParserFromProto(const Proto::Player& proto)
{
	auto& protoBase = proto.base();
	auto& protoPack = proto.pack();
	_gender = protoBase.gender();
	lv = protoBase.level();
	xp = protoBase.xp();
	hp = protoBase.hp();
	mp = protoBase.mp();
	atk = protoBase.atk();
	def = protoBase.def();
	gold = protoPack.gold();
}

void PlayerComponentDetail::SerializeToProto(Proto::Player* pProto)
{
	pProto->mutable_base()->set_level(lv);
	pProto->mutable_base()->set_xp(xp);
	pProto->mutable_base()->set_hp(hp);
	pProto->mutable_base()->set_mp(mp);
	pProto->mutable_base()->set_atk(atk);
	pProto->mutable_base()->set_def(def);
	pProto->mutable_pack()->set_gold(gold);
}

Proto::Gender PlayerComponentDetail::GetGender() const
{
	return _gender;
}
