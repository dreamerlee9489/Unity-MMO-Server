#include "player_component_detail.h"
#include "libplayer/player.h"
#include "libserver/message_system_help.h"

void PlayerComponentDetail::Awake()
{
	pKnap = new std::vector<ItemData>();
	pIdxMap = new std::unordered_map<uint64, int>();
	Player* pPlayer = dynamic_cast<Player*>(_parent);
	ParserFromProto(pPlayer->GetPlayerProto());
}

void PlayerComponentDetail::BackToPool()
{
	delete pKnap;
	delete pIdxMap;
}

void PlayerComponentDetail::ParserFromProto(const Proto::Player& proto)
{
	auto& protoBase = proto.base();
	auto& protoKnap = proto.knap();
	auto& protoMisc = proto.misc();
	_gender = protoBase.gender();
	lv = protoBase.level();
	xp = protoBase.xp();
	hp = protoBase.hp();
	mp = protoBase.mp();
	atk = protoBase.atk();
	def = protoBase.def();
	//for (uint64 sn : protoMisc.team())
	//	team.emplace_back(sn);
	gold = protoKnap.gold();
	for (auto &item : protoKnap.bag_items())
	{
		pIdxMap->emplace(item.sn(), pKnap->size());
		switch (item.type())
		{
		case Proto::ItemData_ItemType_Potion:
			pKnap->emplace_back(ItemType::Potion, item.id(), item.index(), item.sn());
			break;
		case Proto::ItemData_ItemType_Weapon:
			pKnap->emplace_back(ItemType::Weapon, item.id(), item.index(), item.sn());
			break;
		default:
			break;
		}
	}
}

void PlayerComponentDetail::SerializeToProto(Proto::Player* pProto)
{
	pProto->mutable_base()->set_level(lv);
	pProto->mutable_base()->set_xp(xp);
	pProto->mutable_base()->set_hp(hp);
	pProto->mutable_base()->set_mp(mp);
	pProto->mutable_base()->set_atk(atk);
	pProto->mutable_base()->set_def(def);
	//pProto->mutable_misc()->clear_team();
	//for (uint64 sn : team)
	//	pProto->mutable_misc()->add_team(sn);
	pProto->mutable_knap()->set_gold(gold);
	auto& oldItems = *pProto->mutable_knap()->mutable_bag_items();
	for (auto& item : oldItems)
	{
		if (pIdxMap->find(item.sn()) != pIdxMap->end())
		{
			int idx = (*pIdxMap)[item.sn()];
			item.set_index((*pKnap)[idx].index);
			(*pKnap)[idx].sn = 0;
		}
	}
	for (auto& item : *pKnap)
		if (item.sn != 0)
			item.SerializeToProto(pProto->mutable_knap()->add_bag_items());
}

Proto::Gender PlayerComponentDetail::GetGender() const
{
	return _gender;
}
