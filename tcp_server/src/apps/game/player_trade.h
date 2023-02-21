#ifndef TRADE_MGR_H
#define TRADE_MGR_H
#include "libserver/common.h"
#include "libserver/message_system_help.h"

struct PlayerTrade
{
	uint64 applicant, responder;
	bool appAck = false, resAck = false;

	PlayerTrade(uint64 applicant, uint64 responder)
	{
		this->applicant = applicant;
		this->responder = responder;
	}

	~PlayerTrade() = default;
};

#endif // !TRADE_MGR_H
