#pragma once

class TPointOrderFrmData : public ISQuoteDataSpi, public TradeSpiEx
{
public:
    TPointOrderFrmData() {};
	~TPointOrderFrmData() { g_pStarApi->UnsubAllQuote(this); };
public:
	virtual void __stdcall OnQuote(const SContract* cont, const SQuoteUpdate* quote);
	virtual void __stdcall OnSpreadQuote(const SSpreadContract* scont) {};


    virtual void __cdecl OnOrder(const TOrder * pOrder);
	virtual void __cdecl OnMatch(const TMatch * pMatch);
    virtual void __cdecl OnPositionTotal(const TTotalPosition * pPosition);
	virtual void __cdecl OnMoney(const TMoney * pMoney);
public:
    void SetContractNo(string sCode, char* UserNo, int iUserSize);
    void RegistHwnd(HWND hwnd)
    {
        m_hOrderFrm = hwnd;
    }
    void UpdataSign(OrderInfo *pstOrderInfo)
    {
        strcpy_s(m_Sign, pstOrderInfo->szSign1);

    }
    bool GetQuoteData(SQuoteSnapShot &QuoteData)
    {
        QuoteData = m_QuoteData;
        return true;
    }
    bool GetPositionData(std::multimap <double, TPosition> &PositionMap)
    {
        PositionMap = m_PositionMap;
        return true;
    }
    bool GetParOrderData(std::multimap <double, TOrder> &ParOrderMap)
    {
        ParOrderMap = m_ParOrderMap;
        return true;
    }
    bool GetConditionData(std::multimap <double, TOrder> &ConditionMap)
    {
        ConditionMap = m_ConditionOrderMap;
        return true;
    }
    bool DealAutoOpenCloseOrder(TSendOrder &stSendOrder,char* UserNo);
    void InsertCloseOrder(TSendOrder &stOrder, int iCover, int iTCover);
    void InsertCancelCover(TSendOrder& stSendOrder);

private:
    bool UpdataQuoteData();
    bool UpdataPositionData(const TContractSingleKey *pPosition);
    bool UpdataOrderData(const TContractSingleKey *pOrder);
	bool UpdataMoney();

private:
    HWND                                            m_hOrderFrm;
    TContractKey                                    m_stConKey;
    std::multimap <double, TPosition>               m_PositionMap;
    std::multimap <double, TOrder>                  m_ParOrderMap;
    std::multimap <double, TOrder>                  m_ConditionOrderMap;

    std::map <string, uint>                         m_ParOrderCoverMap;
    std::map <string, TSendOrder>                   m_CoverOrderMap;


    TDataContainer                                  m_clPosition;
    TDataContainer                                  m_clParOrder;

    char                                            m_Sign[21];
    char                                           m_UserNo[21];
	SQuoteSnapShot                                  m_QuoteData;
};