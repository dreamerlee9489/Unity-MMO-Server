#ifndef TRADE_MGR_H
#define TRADE_MGR_H
#include "libserver/common.h"
#include "libserver/message_system_help.h"

struct Trade
{
	uint64 applicant, responder;
	bool appAck = false, resAck = false;

	Trade(uint64 applicant, uint64 responder)
	{
		this->applicant = applicant;
		this->responder = responder;
	}

	~Trade() = default;
};

#endif // !TRADE_MGR_H
