#ifndef _TSTRING_CONVERT_H
#define _TSTRING_CONVERT_H

bool MByteToWChar(LPCSTR lpcszStr, LPWSTR lpwszStr, DWORD dwSize);
bool WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD dwSize, UINT codepage = 936);
void CharToWChar(char* Temp, wchar_t* wTemp);
#define DrawWideText(hdc,text,rect,dword)		\
{				\
	char szmbyte[1024]; sprintf_s(szmbyte,"%s",text);	\
	DWORD dwsize = 1024; wchar_t sztext[1024];	\
	MByteToWChar(szmbyte, sztext, dwsize);		\
	DrawText(hdc, sztext, wcslen(sztext), &rect, dword);	\
}
//
std::string strGetOrderState(int i);
std::string strGetOrderWay(int i);
std::string strGetMarket(int i);
std::string strGetTradeD(int i);
std::string strGetLiquidate(int i);
//×ª»»
void		TOrderState2Char(TOrderState& tstate, TStrategyType& ttype, char* sztext, int nsize = MAX_PATH);
void		TDirect2Char(TDirect& tdirect, char* sztext, int nsize = MAX_PATH);
void		TOffset2Char(TOffset& toffset, char* sztext, int nsize = MAX_PATH);
void		TDouble2Char(double dbprice, const char* szformat, char* sztext, int nsize = MAX_PATH);
void		TInt2Char(int nvol, const char* szformat, char* sztext, int nsize = MAX_PATH);
void		TOrderType2Char(TOrderType ttype, char* sztext, int nsize = MAX_PATH);
void		TTriggerMode2Char(TTriggerMode& tmode, char* sztext, int nsize = MAX_PATH);
void		TTriggerCondition2Char(TTriggerCondition& tcondition, char* sztext, int nsize = MAX_PATH);
void		THedge2Char(THedge& thedge, char* sztext, int nsize = MAX_PATH);
void		TTradeSection2Char(TTradeSection& tsection, char* sztext, int nsize = MAX_PATH);
void		TValidType2Char(TValidType& ttype, char* sztext, int nsize = MAX_PATH);
void		TOrderWay2Char(TOrderWay& tway, char* sztext, int nsize = MAX_PATH);
void		TCommodityType2Char(TCommodityType ttype, char* sztext, int nsize = MAX_PATH);
void		TErrorInfo2Char(int nerrorcode, char* errortext, char* sztext, int nsize = MAX_PATH);
void		TStrategyType2Char(TStrategyType& ttype, char* sztext, int nsize = MAX_PATH);
void		TBool2Char(TBool& tbool, char* sztext, int nsize = MAX_PATH);
void		TCheckMode2Char(TCheckMode tmode, char* sztext, int nsize = MAX_PATH);
void		TCheckState2Char(TCheckState tstate, char* sztext, int nsize = MAX_PATH);
void		TStrokeTime2Char(std::string szsrc, int nbegin, int nlength, char* sztext, int nsize = MAX_PATH);
//void		TItemType2Char(ItemType& itemtype, char* sztext, int nsize = MAX_PATH);
//void		TProfitState2Char(tstate st, char* sztext, int nsize = MAX_PATH);

void		TCharFormat(const char* psztext, const char* szformat,char* sztext, int nsize = MAX_PATH);
void		TWchar2Char(const wchar_t* pwsztext,char* sztext,int ndessize = MAX_PATH);

void        TStopState2Char(char orderState, char* szText, int Size);
void        TStopOrderType2Char(char orderType, char* szText, int Size);
void        TStopOrderPrice2Char(int iPriceType, char* szText, int Size);

void		TConOrderState2Char(int state, char * szText, int length);
void        TConOrderType2Char(int state, char*szText, int length);
UINT		GetPriceType(wstring &wstrType);

#endif