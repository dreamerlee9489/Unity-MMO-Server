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
	_gender = protoBase.gender();
	lv = protoBase.level();
	xp = protoBase.xp();
	hp = protoBase.hp();
	mp = protoBase.mp();
	atk = protoBase.atk();
	def = protoBase.def();
	gold = protoKnap.gold();
	for (auto &item : protoKnap.items())
	{
		pIdxMap->emplace(item.sn(), pKnap->size());
		pKnap->emplace_back(ItemData().ParserFromProto(item));
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
	pProto->mutable_knap()->set_gold(gold);
	auto& oldItems = *pProto->mutable_knap()->mutable_items();
	std::list<uint64> rmvSns;
	for (int i = 0; i < oldItems.size(); ++i)
	{
		if (pIdxMap->find(oldItems[i].sn()) != pIdxMap->end())
		{
			int idx = (*pIdxMap)[oldItems[i].sn()];
			if ((*pKnap)[idx].knapType == KnapType::World)
				rmvSns.push_back((*pKnap)[idx].sn);
			else
			{
				(*pKnap)[idx].sn = 0;
				oldItems[i].set_knaptype((Proto::ItemData_KnapType)(*pKnap)[idx].knapType);
				oldItems[i].set_index((*pKnap)[idx].index);
			}
		}
	}
	for (uint64& sn : rmvSns)
	{
		for (auto iter = oldItems.begin(); iter != oldItems.end(); ++iter)
		{
			if ((*iter).sn() == sn)
			{
				oldItems.erase(iter);
				break;
			}
		}
	}
	for (auto& item : *pKnap)
		if (item.sn != 0)
			item.SerializeToProto(pProto->mutable_knap()->add_items());
}

Proto::Gender PlayerComponentDetail::GetGender() const
{
	return _gender;
}
