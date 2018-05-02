#ifndef _TRADE_QUOTE_H_
#define _TRADE_QUOTE_H_

namespace CConvert
{
	bool IsLME(TExchange&Exchange);
	bool TradeContractToStr(const TContractKey & ContractKey, string & strCode, bool bOnlyContract = false, bool bName = false, bool bOnlyName = false);//
	bool StrToTradeSingleContract(const char* szCode, TContractSingleKey& ContractKey);
	vector<string> SplitString(const string & str, char cSplit);
	void  TStopOrderPrice2Char(int iPriceType, char* szText, int Size);
	//QueuePrice排队价 CounterPrice对手价  SuperPrice超价   MarketPrice市价  LastPrice最新价
	void TradeSingleContractToRawId(TContractSingleKey* ContractSingleKey, char(&ContractId)[51]);
	TCommodityType TradeTypeToQuoteType(char cType);
	void TradeContractToRawContractIdType(const TContractKey &ContractKey, char(&ContractId)[51]);
	bool StrToTradeContract(const char * szCode, TContractKey & ContractKey);//保证传过来的字符串左右两端都去掉空格了
	void DefaultSendOrder(TSendOrder&SendOrder);
	//获取持仓
	void GetPositionVol(const char * UserNo, const TContractKey * ContractKey
		, uint &LongCount, uint &ShortCount, uint &DayLongCount, uint &DayShortCount);
	bool IsCombContract(const char CommodityType);
	void CombContractToSingleContract(const TContractKey &ContractKey, TContractSingleKey &SigCont1, TContractSingleKey &SigCont2);
	TOffset GetOffset(const char* UserNo, const TContractKey * ContractKey, TDirect direct);
	bool GetCommodityInf(const TCommodityKey &sCommodityKey, SOFCommodityInf & sCommodityInfo);
	void GetCloseNum(char* UserNo, TContractSingleKey* ContractKey, TDirect direct, uint &iCoverQty, int& iCoverNum, int& iTCoverNum);//新增下单数量自适应
	bool IsDistinguishCoverType(const TCommodityKey*pCommoditykey);
	bool CombContractIsFirst(const TCommodityKey & CommodityKey);
	bool newGUID(char* buf);

	void GetFilePath(TCHAR* filePath, int length, TCHAR* fileName);
	void GetFilePath(char* filePath, int length, char* filenName);
	void DefaultDealClose(TPosition *stPosition, TSendOrder &stOrder);
	bool Cmp2ContractSingleKey(TContractSingleKey key1, TContractSingleKey key2);

    bool ParOrderToStr(const TSendOrder & stOrder, string & strCode);
    //void GetAutoCloseNum(char* UserNo, TContractSingleKey* ContractKey, TDirect direct, const int iCoverQty, int& iCoverNum, int& iTCoverNum,int &iPositionNum);

	//分割字符串
	bool SplitNotice(const std::string &strContent, char cSplit, std::vector<NoticeContent> &vecstr);
}

#endif