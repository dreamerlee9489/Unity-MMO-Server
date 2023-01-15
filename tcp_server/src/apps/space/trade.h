#ifndef TRADE_MGR_H
#define TRADE_MGR_H
#include "libserver/common.h"
#include "libserver/message_system_help.h"

struct Trade
{
	int ack = 0;
	uint64 applicant, responder;

	Trade() = default;

	Trade(uint64 applicant, uint64 responder)
	{
		this->applicant = applicant;
		this->responder = responder;
	}
};

//class TradeMgr
//{
//public:
//	static TradeMgr* GetInstance() 
//	{
//		if (!_instance)
//			_instance = new TradeMgr();
//		return _instance;
//	}
//
//	void MakeTrade(Proto::PlayerReq& proto, Player* pApp, Player* pRes)
//	{
//		//Trade* pTrade = new Trade(pApp->GetPlayerSN(), pRes->GetPlayerSN());
//		//tradeMap.emplace(pApp->GetPlayerSN(), pTrade);
//		//tradeMap.emplace(pRes->GetPlayerSN(), pTrade);
//		//MessageSystemHelp::SendPacket(Proto::MsgId::C2C_TradeRes, proto, pApp);
//		//MessageSystemHelp::SendPacket(Proto::MsgId::C2C_TradeRes, proto, pRes);
//	}
//
//	void UpdateKnapItem(Player* pPlayer, Proto::UpdateKnapItem& proto)
//	{
//		//Player* target = nullptr;
//		//if (pPlayer->GetPlayerSN() == tradeMap[pPlayer->GetPlayerSN()]->applicant)
//		//	target = playerMgr->GetPlayerBySn(tradeMap[pPlayer->GetPlayerSN()]->responder);
//		//else
//		//	target = playerMgr->GetPlayerBySn(tradeMap[pPlayer->GetPlayerSN()]->applicant);
//		//MessageSystemHelp::SendPacket(Proto::MsgId::S2C_UpdateKnapItem, proto, target);
//	}
//
//private:
//	TradeMgr() {}
//
//	static TradeMgr* _instance;	
//};
//
//TradeMgr* TradeMgr::_instance = nullptr;

#endif // !TRADE_MGR_H
