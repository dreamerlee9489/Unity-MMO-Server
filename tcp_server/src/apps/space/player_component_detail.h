#pragma once
#include "libserver/component.h"
#include "libserver/system.h"
#include "libplayer/player_component.h"

class PlayerComponentDetail :public Component<PlayerComponentDetail>, public IAwakeFromPoolSystem<>, public PlayerComponent
{
public:
	int lv = 1, xp = 0, hp = 999, mp = 999, atk = 10, def = 0, gold = 500;

	void Awake() override;
	void BackToPool() override;

	void ParserFromProto(const Proto::Player& proto) override;
	void SerializeToProto(Proto::Player* pProto) override;

	Proto::Gender GetGender() const;

private:
	Proto::Gender _gender;
};

