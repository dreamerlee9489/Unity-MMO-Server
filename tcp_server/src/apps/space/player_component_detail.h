#pragma once
#include "libserver/component.h"
#include "libserver/system.h"
#include "libplayer/player_component.h"

class PlayerComponentDetail :public Component<PlayerComponentDetail>, public IAwakeFromPoolSystem<>, public PlayerComponent
{
public:
	void Awake() override;
	void BackToPool() override;

	void ParserFromProto(const Net::Player& proto) override;
	void SerializeToProto(Net::Player* pProto) override;

	Net::Gender GetGender() const;

private:
	Net::Gender _gender;
};

