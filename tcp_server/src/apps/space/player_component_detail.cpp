#include "player_component_detail.h"
#include "libplayer/player.h"
#include "libserver/message_system_help.h"

void PlayerComponentDetail::Awake()
{
	pKnap = new std::list<DropItem>();
	Player* pPlayer = dynamic_cast<Player*>(_parent);
	ParserFromProto(pPlayer->GetPlayerProto());
}

void PlayerComponentDetail::BackToPool()
{
	delete pKnap;
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
		switch (item.type())
		{
		case Proto::ItemData_ItemType_Potion:
			pKnap->emplace_back(DropItem(ItemType::Potion, item.id(), item.num()));
			break;
		case Proto::ItemData_ItemType_Weapon:
			pKnap->emplace_back(DropItem(ItemType::Weapon, item.id(), item.num()));
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
	pProto->mutable_knap()->set_gold(gold);
	auto& oldItems = *pProto->mutable_knap()->mutable_items();
	for (auto& item : oldItems)
	{
		for (auto& iter = pKnap->begin(); iter != pKnap->end(); ++iter)
		{
			if ((int)(*iter).type == item.type() && (*iter).id == item.id())
			{
				item.set_num((*iter).num);
				(*pKnap).erase(iter);
				break;
			}
		}
	}
	for (auto& item : *pKnap)
	{
		Proto::ItemData* itemData = pProto->mutable_knap()->add_items();
		itemData->set_id(item.id);
		itemData->set_num(item.num);
		switch (item.type)
		{
		case ItemType::Potion:
			itemData->set_type(Proto::ItemData_ItemType_Potion);
			break;
		case ItemType::Weapon:
			itemData->set_type(Proto::ItemData_ItemType_Weapon);
			break;
		default:
			break;
		}
	}
}

Proto::Gender PlayerComponentDetail::GetGender() const
{
	return _gender;
}
