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
	for (auto &item : protoKnap.itemsinbag())
	{
		switch (item.type())
		{
		case Proto::ItemData_ItemType_Potion:
			pKnap->emplace_back(ItemType::Potion, item.id(), item.num(), item.index(), item.hash());
			break;
		case Proto::ItemData_ItemType_Weapon:
			pKnap->emplace_back(ItemType::Weapon, item.id(), item.num(), item.index(), item.hash());
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
	auto& oldItems = *pProto->mutable_knap()->mutable_itemsinbag();
	for (auto& item : oldItems)
	{
		for (auto& iter = pKnap->begin(); iter != pKnap->end(); ++iter)
		{
			if ((*iter).hash == item.hash())
			{
				item.set_num((*iter).num);
				item.set_index((*iter).index);
				pKnap->erase(iter);
				break;
			}
		}
	}
	for (auto& item : *pKnap)
	{
		Proto::ItemData* itemData = pProto->mutable_knap()->add_itemsinbag();
		itemData->set_id(item.id);
		itemData->set_num(item.num);
		itemData->set_index(item.index);
		itemData->set_hash(item.hash);
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
