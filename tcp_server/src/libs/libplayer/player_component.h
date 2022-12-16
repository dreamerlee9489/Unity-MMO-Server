#pragma once

#include "libserver/common.h"

class PlayerComponent
{
public:
	virtual void ParserFromProto(const Net::Player& proto) = 0;
	virtual void SerializeToProto(Net::Player* pProto) = 0;
};

