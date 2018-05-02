#include "PreInclude.h"

//
#define MI_10			10000000000			
bool MByteToWChar(LPCSTR lpcszStr, LPWSTR lpwszStr, DWORD dwSize)
{
	// Get the required size of the buffer that receives the Unicode 
	// string. 
	DWORD dwMinSize;
	dwMinSize = MultiByteToWideChar(936, 0, lpcszStr, -1, NULL, 0);

	if (dwSize < dwMinSize)
	{
		return false;
	}


	// Convert headers from ASCII to Unicode.
	MultiByteToWideChar(936, 0, lpcszStr, -1, lpwszStr, dwMinSize);
	return true;
}

bool WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD dwSize, UINT codepage/* = 936*/)
{
	DWORD dwMinSize;
	dwMinSize = WideCharToMultiByte(codepage, NULL, lpcwszStr, -1, NULL, 0, NULL, FALSE);
	if (dwSize < dwMinSize)
		return false;
	WideCharToMultiByte(codepage, NULL, lpcwszStr, -1, lpszStr, dwSize, NULL, FALSE);
	return true;
}
void CharToWChar(char* Temp, wchar_t* wTemp)
{
	UINT nPage = 936;
	int  unicodeLen = ::MultiByteToWideChar(nPage, 0, Temp, -1, nullptr, 0);
	wTemp = new  wchar_t[unicodeLen + 1];
	memset(wTemp, 0, (unicodeLen + 1)*sizeof(wchar_t));
	::MultiByteToWideChar(nPage, 0, Temp, -1, (LPWSTR)wTemp, unicodeLen);	
}
//TOrderState
char		szorderstatechs[][50] = {
	"无", "已发送", "已受理", "待触发", "埋单",
	"已生效", "已排队", "部分成交", "完全成交", "待撤",
	"待改", "已撤单", "已撤余单", "指令失败", "待审核",
	"已挂起", "已申请", "无效单", "部分触发", "完全触发",
	"余单失败"
};
char		szorderstatecht[][50] = {
	"無", "已發送", "已受理", "待觸發", "埋單",
	"已生效", "已排隊", "部分成交", "完全成交", "待撤",
	"待改", "已撤單", "已撤余單", "指令失敗", "待審核",
	"已掛起", "已申請", "無效單", "部分觸發", "完全觸發",
	"余單失敗"
};
char		szorderstateenu[][50] = {
	"NONE", "Sended", "Accepted", "To be triggered", "Pre-hour order",
	"Executed", "In queue", "Partially Filled", "Filled", "To be cancelled",
	"To be modified", "Cancelled", "Remaining orders Cancelled", "Instruction Rejected", "Checking",
	"Suspended", "Applied", "Invalid Order", "Partially Triggered", "Triggered",
	"Remaining orders Rejected"
};
char* _GetOrderState(int i)
{
	if (g_language == ENU) return szorderstateenu[i];
	else if (g_language == CHT) return szorderstatecht[i];
	return szorderstatechs[i];
}
void		TOrderState2Char(TOrderState& tstate, TStrategyType& ttype, char* sztext, int nsize/* = MAX_PATH*/)
{
	switch (tstate)
	{
	case  osNone: strcpy_s(sztext, nsize, _GetOrderState(0)); break;//					'N' //无
	case	osSended: strcpy_s(sztext, nsize, _GetOrderState(1)); break;//				'0'	//已发送
	case	osAccept: strcpy_s(sztext, nsize, _GetOrderState(2)); break;//				'1'	//已受理
	case osTriggering: //			'2'	//待触发
		if (ttype != stPreOrder)
			strcpy_s(sztext, nsize, _GetOrderState(3));
		else
			strcpy_s(sztext, nsize, _GetOrderState(4));
		break;
	case	osActive: strcpy_s(sztext, nsize, _GetOrderState(5)); break;//				'3'	//已生效
	case	osQueued: strcpy_s(sztext, nsize, _GetOrderState(6)); break;//			'4'	//已排队
	case	osPartFilled: strcpy_s(sztext, nsize, _GetOrderState(7)); break;//			'5'	//部分成交
	case	osFilled: strcpy_s(sztext, nsize, _GetOrderState(8)); break;//			'6'	//完全成交
	case	osCanceling: strcpy_s(sztext, nsize, _GetOrderState(9)); break;//				'7'	//待撤
	case	osModifying: strcpy_s(sztext, nsize, _GetOrderState(10)); break;//				'8'	//待改
	case	osCanceled: strcpy_s(sztext, nsize, _GetOrderState(11)); break;//				'9'	//已撤单
	case	osPartCanceled: strcpy_s(sztext, nsize, _GetOrderState(12)); break;//				'A'	//已撤余单
	case	osFail: strcpy_s(sztext, nsize, _GetOrderState(13)); break;//				'B'	//指令失败
	case osChecking: strcpy_s(sztext, nsize, _GetOrderState(14)); break;//				'C' //待审核
	case	osSuspended: strcpy_s(sztext, nsize, _GetOrderState(15)); break;//				'D'	//已挂起
	case	osApply: strcpy_s(sztext, nsize, _GetOrderState(16)); break;//					'E'	//已申请
	case osInvalid: strcpy_s(sztext, nsize, _GetOrderState(17)); break;//				'F' //无效单
	case osPartTriggered:strcpy_s(sztext, nsize, _GetOrderState(18)); break;//'G' //部分触发
	case osFillTriggered:strcpy_s(sztext, nsize, _GetOrderState(19)); break;//			'H' //完全触发
	case osPartFailed:strcpy_s(sztext, nsize, _GetOrderState(20)); break;//'I' //余单失败
	}
}

std::string strGetOrderState(int i)
{
	std::string sztext;
	switch (i)
	{
	case 0: sztext = _GetOrderState(0); break;//					'N' //无
	case	1: sztext = _GetOrderState(1); break;//				'0'	//已发送
	case	2: sztext = _GetOrderState(2); break;//				'1'	//已受理
	case 3: sztext = _GetOrderState(3); break;
	case	4: sztext = _GetOrderState(5); break;//				'3'	//已生效
	case	5: sztext = _GetOrderState(6); break;//			'4'	//已排队
	case	6: sztext = _GetOrderState(7); break;//			'5'	//部分成交
	case	7: sztext = _GetOrderState(8); break;//			'6'	//完全成交
	case	8: sztext = _GetOrderState(9); break;//				'7'	//待撤
	case	9: sztext = _GetOrderState(10); break;//				'8'	//待改
	case	10: sztext = _GetOrderState(11); break;//				'9'	//已撤单
	case	11: sztext = _GetOrderState(12); break;//				'A'	//已撤余单
	case	12: sztext = _GetOrderState(13); break;//				'B'	//指令失败
	case 13: sztext = _GetOrderState(14); break;//				'C' //待审核
	case	14: sztext = _GetOrderState(15); break;//				'D'	//已挂起
	case	15: sztext = _GetOrderState(16); break;//					'E'	//已申请
	case 16: sztext = _GetOrderState(17); break;//				'F' //无效单
	case 17: sztext = _GetOrderState(18); break;//				'F' //部分触发
	case 18: sztext = _GetOrderState(19); break;//				'F' //完全触发
	case 19: sztext = _GetOrderState(20); break;//				'F' //余单失败

	default: sztext = "";
	}

	return sztext;
}

//////////////////////////////////////////////////////////////////////////
char		szdirectchs[][50] = {
	"买入", "卖出", "双边"
};
char		szdirectcht[][50] = {
	"買入", "賣出", "雙邊"
};
char		szdirectenu[][50] = {
	"Buy", "Sell", "Bilateral"
};
char* _GetDirect(int i)
{
	if (g_language == ENU) return szdirectenu[i];
	else if (g_language == CHT) return szdirectcht[i];
	return szdirectchs[i];
}

void		TDirect2Char(TDirect& tdirect, char* sztext, int nsize)
{
	switch (tdirect)
	{
	case dNone: strcpy_s(sztext, nsize, ""); break;// sztext = ""; break;//					'N'
	case	dBuy: strcpy_s(sztext, nsize, _GetDirect(0)); break; //sztext = _GetDirect(0); break;//				'B'	//买入
	case	dSell: strcpy_s(sztext, nsize, _GetDirect(1)); break; //sztext = _GetDirect(1); break;//				'S'	//卖出
	case	dBoth: strcpy_s(sztext, nsize, _GetDirect(2)); break; //sztext = _GetDirect(2); break;//				'A'	//双边
	}
}

//////////////////////////////////////////////////////////////////////////
char		szoffsetchs[][50] = {
	"开仓", "平仓", "平今",
	"开平", "平开"
};
char		szoffsetcht[][50] = {
	"開倉", "平倉", "平今",
	"開平", "平開"
};
char		szoffsetenu[][50] = {
	"Open", "Close", 	"Close today",
	"O/C", "Close and open"
};
char* _GetOffset(int i)
{
	if (g_language == ENU) return szoffsetenu[i];
	else if (g_language == CHT) return szoffsetcht[i];
	return szoffsetchs[i];
}
void		TOffset2Char(TOffset& toffset, char* sztext, int nsize)
{
	switch (toffset)
	{
	case	oNone: strcpy_s(sztext, nsize, _GetOrderState(0)); break;// sztext = _GetOrderState(0); break;//				'N'	//无
	case	oOpen: strcpy_s(sztext, nsize, _GetOffset(0)); break;// sztext = _GetOffset(0); break;//				'O'	//开仓
	case	oCover: strcpy_s(sztext, nsize, _GetOffset(1)); break;// sztext = _GetOffset(1); break;//				'C'	//平仓
	case	oCoverT: strcpy_s(sztext, nsize, _GetOffset(2)); break;//sztext = _GetOffset(2); break;//				'T'	//平今
	case oOpenCover: strcpy_s(sztext, nsize, _GetOffset(3)); break;//sztext = _GetOffset(3); break;//				'1' //开平，应价时有效
	case oCoverOpen: strcpy_s(sztext, nsize, _GetOffset(4)); break;//sztext = _GetOffset(4); break;//				'2'	//平开，应价时有效
	}
}

void		TDouble2Char(double dbprice, const char* szformat, char* sztext, int nsize)
{
	if (sztext == nullptr || szformat == nullptr) return;
	if (abs(dbprice) > MI_10)
		sprintf_s(sztext, nsize, "%ef", dbprice);
	else
		sprintf_s(sztext, nsize, szformat, dbprice);
}

void		TInt2Char(int nvol, const char* szformat, char* sztext, int nsize)
{
	if (sztext == nullptr || szformat == nullptr) return;
	sprintf_s(sztext, nsize, szformat, nvol);
}

void		TCharFormat(const char* psztext, const char* szformat, char* sztext, int nsize)
{
	if (sztext == nullptr || szformat == nullptr) return;
	sprintf_s(sztext, nsize, szformat, psztext);
}

void		TWchar2Char(const wchar_t* pwsztext, char* sztext, int ndessize/* = MAX_PATH*/)
{
	WCharToMByte(pwsztext, sztext, ndessize);
}
//////////////////////////////////////////////////////////////////////////
char		szordertypechs[][50] = {
	"未定义", "市价单", "限价单",
	"市价止损", "限价止损", "行权",
	"弃权", "询价", "应价",
	"冰山单", "影子单", "互换",
	"套利申请", "套保申请"
};
char		szordertypecht[][50] = {
	"未定義", "市價單", "限價單",
	"市價止損", "限價止損", "行權",
	"棄權", "詢價", "應價",
	"冰山單", "影子單", "互換",
	"套利申請", "套保申請"
};
char		szordertypeenu[][50] = {
	"Undefine", "Market Order", "Limited Price Order",
	"Stop loss at Market", "Stop loss at limit price", "Exercise",
	"Waiver", "Enquiry", "Bidding price",
	"Iceberg Order", "Shadow Order", "Swap",
	"Spread Application", "Hedging application"
};
char* _GetOrdertype(int i)
{
	if (g_language == ENU) return szordertypeenu[i];
	else if (g_language == CHT) return szordertypecht[i];
	return szordertypechs[i];
}
void TOrderType2Char(TOrderType ttype, char* sztext, int nsize)
{
	switch (ttype)
	{
	case otUnDefine: strcpy_s(sztext, nsize, _GetOrdertype(0)); break;// sztext = _GetOrdertype(0); break;//					'U' //未定义
	case	otMarket: strcpy_s(sztext, nsize, _GetOrdertype(1)); break;//sztext = _GetOrdertype(1); break;//				'1'	//市价单
	case	otLimit: strcpy_s(sztext, nsize, _GetOrdertype(2)); break;//sztext = _GetOrdertype(2); break;//					'2'	//限价单
	case	otMarketStop: strcpy_s(sztext, nsize, _GetOrdertype(3)); break;//sztext = _GetOrdertype(3); break;//			'3'	//市价止损
	case	otLimitStop: strcpy_s(sztext, nsize, _GetOrdertype(4)); break;//sztext = _GetOrdertype(4); break;//				'4'	//限价止损
	case otExecute: strcpy_s(sztext, nsize, _GetOrdertype(5)); break;//sztext = _GetOrdertype(5); break;//				'5' //行权
	case otAbandon: strcpy_s(sztext, nsize, _GetOrdertype(6)); break;//sztext = _GetOrdertype(6); break;//				'6' //弃权
	case otEnquiry: strcpy_s(sztext, nsize, _GetOrdertype(7)); break;//sztext = _GetOrdertype(7); break;//				'7' //询价
	case otOffer: strcpy_s(sztext, nsize, _GetOrdertype(8)); break;//sztext = _GetOrdertype(8); break;//				'8' //应价
	case otIceberg: strcpy_s(sztext, nsize, _GetOrdertype(9)); break;//sztext = _GetOrdertype(9); break;//				'9' //冰山单
	case otGhost: strcpy_s(sztext, nsize, _GetOrdertype(10)); break;//sztext = _GetOrdertype(10); break;//					'A' //影子单
	case otSwap: strcpy_s(sztext, nsize, _GetOrdertype(11)); break;//sztext = _GetOrdertype(11); break;//				'B' //互换
	case otSpreadApply: strcpy_s(sztext, nsize, _GetOrdertype(12)); break;//sztext = _GetOrdertype(12); break;//			'C' //套利申请
	case otHedgApply: strcpy_s(sztext, nsize, _GetOrdertype(13)); break;//sztext = _GetOrdertype(13); break;//				'D' //套保申请
	}
}

//////////////////////////////////////////////////////////////////////////
char		sztriggermodechs[][50] = {
	"最新价", "买价", "卖价"
};
char		sztriggermodecht[][50] = {
	"最新價", "買價", "賣價"
};
char		sztriggermodeenu[][50] = {
	"Latest Price", "Bid Price", "Ask Price"
};
char* _GetTriggerMode(int i)
{
	if (g_language == ENU) return sztriggermodeenu[i];
	else if (g_language == CHT) return sztriggermodecht[i];
	return sztriggermodechs[i];
}
void		TTriggerMode2Char(TTriggerMode& tmode, char* sztext, int nsize)
{
	switch (tmode)
	{
	case tmNone: strcpy_s(sztext, nsize, _GetOrderState(0)); break;//sztext = _GetOrderState(0); break;//				'N' //无
	case tmLatest: strcpy_s(sztext, nsize, _GetTriggerMode(0)); break;//sztext = _GetTriggerMode(0); break;//		'L' //最新价
	case tmBid: strcpy_s(sztext, nsize, _GetTriggerMode(1)); break;//sztext = _GetTriggerMode(1); break;//				'B' //买价
	case tmAsk: strcpy_s(sztext, nsize, _GetTriggerMode(2)); break;//sztext = _GetTriggerMode(2); break;//				'A' //卖价
	}
}

/////////////////////////////////////////////////////////////////////////
char		sztriggerconditionchs[][50] = {
	"大于", "大于等于", "小于",
	"小于等于"
};
char		sztriggerconditioncht[][50] = {
	"大於", "大於等於", "小於",
	"小於等於"
};
char		sztriggerconditionenu[][50] = {
	"Greater", "Greater or Equal", "Less",
	"Less or Equal"
};
char* _GetTriggerCondition(int i)
{
	if (g_language == ENU) return sztriggerconditionenu[i];
	else if (g_language == CHT) return sztriggerconditioncht[i];
	return sztriggerconditionchs[i];
}

void		TTriggerCondition2Char(TTriggerCondition& tcondition, char* sztext, int nsize)
{
	switch (tcondition)
	{
	case tcNone: strcpy_s(sztext, nsize, _GetOrderState(0)); break;// sztext = _GetOrderState(0); break;//				'N' //无
	case tcGreater: strcpy_s(sztext, nsize, _GetTriggerCondition(0)); break;//sztext = _GetTriggerCondition(0); break;//			'g' //大于
	case tcGreaterEqual: strcpy_s(sztext, nsize, _GetTriggerCondition(1)); break;//sztext = _GetTriggerCondition(1); break;//		'G' //大于等于
	case tcLess: strcpy_s(sztext, nsize, _GetTriggerCondition(2)); break;//sztext = _GetTriggerCondition(2); break;//			'l' //小于
	case tcLessEqual: strcpy_s(sztext, nsize, _GetTriggerCondition(3)); break;//sztext = _GetTriggerCondition(3); break;//			'L' //小于等于 }
	}
}

/////////////////////////////////////////////////////////////////////////
char		szhedgechs[][50] = {
	"投机", "套保", "套利",
	"做市"
};
char		szhedgecht[][50] = {
	"投機", "套保", "套利",
	"做市"
};
char		szhedgeenu[][50] = {
	"Speculate", "Hedge", "Spread",
	"Market maker"
};
char* _GetHedge(int i)
{
	if (g_language == ENU) return szhedgeenu[i];
	else if (g_language == CHT) return szhedgecht[i];
	return szhedgechs[i];
}

void		THedge2Char(THedge& thedge, char* sztext, int nsize)
{
	switch (thedge)
	{
	case	hNone: strcpy_s(sztext, nsize, _GetOrderState(0)); break;//sztext = _GetOrderState(0); break;//				'N'	//无
	case	hSpeculate: strcpy_s(sztext, nsize, _GetHedge(0)); break;//sztext = _GetHedge(0); break;//			'T'	//投机
	case	hHedge: strcpy_s(sztext, nsize, _GetHedge(1)); break;//sztext = _GetHedge(1); break;//				'B'	//套保
	case hSpread: strcpy_s(sztext, nsize, _GetHedge(2)); break;//sztext = _GetHedge(2); break;//			'S' //套利
	case hMarket: strcpy_s(sztext, nsize, _GetHedge(3)); break;//sztext = _GetHedge(3); break;//				'M' //做市
	}
}

/////////////////////////////////////////////////////////////////////////
char		sztradesectionchs[][50] = {
	"白天", "晚上(T+1)", "全交易"
};
char		sztradesectioncht[][50] = {
	"白天", "晚上(T+1)", "全交易"
};
char		sztradesectionenu[][50] = {
	"Day", "Night(T+1)", "All",
	"Full Time"
};
char* _GetTradeSection(int i)
{
	if (g_language == ENU) return sztradesectionenu[i];
	else if (g_language == CHT) return sztradesectioncht[i];
	return sztradesectionchs[i];
}
void		TTradeSection2Char(TTradeSection& tsection, char* sztext, int nsize)
{
	switch (tsection)
	{
	case tsDay: strcpy_s(sztext, nsize, _GetTradeSection(0)); break;//sztext = _GetTradeSection(0); break;//				'D' //白天交易时段
	case tsNight: strcpy_s(sztext, nsize, _GetTradeSection(1)); break;//sztext = _GetTradeSection(1); break;//			'N' //晚上（T+1）交易时段
	case tsAll: strcpy_s(sztext, nsize, _GetTradeSection(2)); break;//sztext = _GetTradeSection(2); break;//			'A' //全交易时段
	}
}

/////////////////////////////////////////////////////////////////////////
char		szvalidtypechs[][50] = {
	"即时全部", "即时部分", "本节有效",
	"当日有效", "长期有效", "限期有效"
};
char		szvalidtypecht[][50] = {
	"即時全部", "即時部分", "本節有效",
	"當日有效", "長期有效", "限期有效"
};
char		szvalidtypeenu[][50] = {
	"FOK", "Ioc", "GFS",
	"GFD", "GTC", "GTD"
};
char* _GetValidType(int i)
{
	if (g_language == ENU) return szvalidtypeenu[i];
	else if (g_language == CHT) return szvalidtypecht[i];
	return szvalidtypechs[i];
}

void		TValidType2Char(TValidType& ttype, char* sztext, int nsize)
{
	switch (ttype)
	{
	case vtNone: strcpy_s(sztext, nsize, _GetOrderState(0)); break;//sztext = _GetOrderState(0); break;//'N' //无
	case	vtFOK: strcpy_s(sztext, nsize, _GetValidType(0)); break;//sztext = _GetValidType(0); break;//'4'	//即时全部
	case	vtIOC: strcpy_s(sztext, nsize, _GetValidType(1)); break;//sztext = _GetValidType(1); break;//'3'	//即时部分
	case vtGFS: strcpy_s(sztext, nsize, _GetValidType(2)); break;//sztext = _GetValidType(2); break;//'5' //本节有效
	case	vtGFD: strcpy_s(sztext, nsize, _GetValidType(3)); break;//sztext = _GetValidType(3); break;//'0'	//当日有效
	case	vtGTC: strcpy_s(sztext, nsize, _GetValidType(4)); break;//sztext = _GetValidType(4); break;//'1'	//长期有效
	case	vtGTD: strcpy_s(sztext, nsize, _GetValidType(5)); break;//sztext = _GetValidType(5);; break;//'2'	//限期有效
	}
}

/////////////////////////////////////////////////////////////////////////
char		szorderwaychs[][50] = {
	"所有", "代理单", "人工单",
	"Carry单", "交割行权", "程式单",
	"行权", "弃权", "通道费",
	"RTS", "本地套利", "横向下单", "竖向下单",
	"点价下单", "卡片下单", "批量下单", "快速下单",
	"乒乓", "止损止盈", "跟单", "追单"
};
char		szorderwaycht[][50] = {
	"所有", "代理單", "人工單",
	"Carry單", "交割行權", "程式單",
	"行權", "棄權", "通道費",
	"RTS", "本地套利", "橫向下單", "豎向下單",
	"點價下單", "卡片下單", "批量下單", "快速下單",
	"乒乓", "止損止盈", "跟單", "追單"
};
char		szorderwayenu[][50] = {
	"All", "Proxy", "Manual Sheet",
	"Carry Order", "Delivery Exercise", "Program Order",
	"Exercise", "Waiver", "Slotting Fee",
	"RTS", "Local Spread", "Place order in Horz window", "Vert order placing",
	"Click price to Place an Order", "Card Ordering", "Batch Order", "Placing Order with Shortcuts",
	"Ping-pong", "Stop-P/L", "Follow-up", "Order-adding"
};
char* _GetOrderWay(int i)
{
	if (g_language == ENU) return szorderwayenu[i];
	else if (g_language == CHT) return szorderwaycht[i];
	return szorderwaychs[i];
}

void		TOrderWay2Char(TOrderWay& tway, char* sztext, int nsize)
{
	switch (tway)
	{
	case owUndefine: strcpy_s(sztext, nsize, _GetOrdertype(0)); break;//sztext = _GetOrdertype(0); break;//				'U' //未定义
	case	owAll: strcpy_s(sztext, nsize, _GetOrderWay(0)); break;//sztext = _GetOrderWay(0); break;//				'A'	//所有
	case	owETrade: strcpy_s(sztext, nsize, "E-Trade"); break;//sztext = "E-Trade"; break;//		'E'	//E-Trade
	case	owProxyETrade: strcpy_s(sztext, nsize, _GetOrderWay(1)); break;//sztext = _GetOrderWay(1); break;//			'P'	//代理单
	case	owJTrade: strcpy_s(sztext, nsize, "J-Trade"); break;//sztext = "J-Trade"; break;//			'J'	//J-Trade
	case	owManual: strcpy_s(sztext, nsize, _GetOrderWay(2)); break;//sztext = _GetOrderWay(2); break;//			'M'	//人工单
	case	owCarry: strcpy_s(sztext, nsize, _GetOrderWay(3)); break;//sztext = _GetOrderWay(3); break;//				'C'	//Carry单
	case	owDelivery: strcpy_s(sztext, nsize, _GetOrderWay(4)); break;//sztext = _GetOrderWay(4); break;//			'D'	//交割行权
	case	owProgram: strcpy_s(sztext, nsize, _GetOrderWay(5)); break;//sztext = _GetOrderWay(5); break;//				'S' //程式单	
	case owExecute: strcpy_s(sztext, nsize, _GetOrderWay(6)); break;//sztext = _GetOrderWay(6); break;//				'e' //行权
	case owAbandon: strcpy_s(sztext, nsize, _GetOrderWay(7)); break;//sztext = _GetOrderWay(7); break;//			'a' //弃权
	case owChannel: strcpy_s(sztext, nsize, _GetOrderWay(8)); break;//sztext = _GetOrderWay(8); break;//			'c' //通道费

	case owRTS: strcpy_s(sztext, nsize, _GetOrderWay(9)); break;//sztext = _GetOrderWay(9); break;//'R' //RTS
//	case owSpreadLocal: strcpy_s(sztext, nsize, _GetOrderWay(10)); break;//sztext = _GetOrderWay(10); break;//'L' //本地套利
	case	owHorz: strcpy_s(sztext, nsize, _GetOrderWay(11)); break;//sztext = _GetOrderWay(11); break;//'H'	//横向下单
	case	owVert: strcpy_s(sztext, nsize, _GetOrderWay(12)); break;//sztext = _GetOrderWay(12); break;//'V'	//竖向下单
	case	owClickPrice: strcpy_s(sztext, nsize, _GetOrderWay(13)); break;//sztext = _GetOrderWay(13); break;//'l'	//点价下单
	case	owCard: strcpy_s(sztext, nsize, _GetOrderWay(14)); break;//sztext = _GetOrderWay(14); break;//'r' //卡片下单	
	case owBrtch: strcpy_s(sztext, nsize, _GetOrderWay(15)); break;//sztext = _GetOrderWay(15); break;//'B' //批量下单
	case owFast: strcpy_s(sztext, nsize, _GetOrderWay(16)); break;//sztext = _GetOrderWay(16); break;//'F' //快速下单
	case owPingPong: strcpy_s(sztext, nsize, _GetOrderWay(17)); break;//sztext = _GetOrderWay(17); break;//'p' //乒乓
//	case	owStop: strcpy_s(sztext, nsize, _GetOrderWay(18)); break;//sztext = _GetOrderWay(18); break;//'s' //止损止盈
	case owFollow: strcpy_s(sztext, nsize, _GetOrderWay(19)); break;//sztext = _GetOrderWay(19); break;//'f' //跟单
	case owCatch: strcpy_s(sztext, nsize, _GetOrderWay(20)); break;//sztext = _GetOrderWay(20); break;//'t' //追单
	}
}

std::string strGetOrderWay(int i)
{
	std::string sztext;
	switch (i)
	{
	case 0: sztext = _GetOrdertype(0); break;//				'U' //未定义
	case	1: sztext = _GetOrderWay(0); break;//				'A'	//所有
	case	2: sztext = "E-Trade"; break;//		'E'	//E-Trade
	case	3: sztext = _GetOrderWay(1); break;//			'P'	//代理单
	case	4: sztext = "J-Trade"; break;//			'J'	//J-Trade
	case	5: sztext = _GetOrderWay(2); break;//			'M'	//人工单
	case	6: sztext = _GetOrderWay(3); break;//				'C'	//Carry单
	case	7: sztext = _GetOrderWay(4); break;//			'D'	//交割行权
	case	8: sztext = _GetOrderWay(5); break;//				'S' //程式单	
	case 9: sztext = _GetOrderWay(6); break;//				'e' //行权
	case 10: sztext = _GetOrderWay(7); break;//			'a' //弃权
	case 11: sztext = _GetOrderWay(8); break;//			'c' //通道费

	case 12: sztext = _GetOrderWay(9); break;//'R' //RTS
	case 13: sztext = _GetOrderWay(10); break;//'L' //本地套利
	case	14: sztext = _GetOrderWay(11); break;//'H'	//横向下单
	case	15: sztext = _GetOrderWay(12); break;//'V'	//竖向下单
	case	16: sztext = _GetOrderWay(13); break;//'l'	//点价下单
	case	17: sztext = _GetOrderWay(14); break;//'r' //卡片下单	
	case 18: sztext = _GetOrderWay(15); break;//'B' //批量下单
	case 19: sztext = _GetOrderWay(16); break;//'F' //快速下单
	case 20: sztext = _GetOrderWay(17); break;//'p' //乒乓
	case	21: sztext = _GetOrderWay(18); break;//'s' //止损止盈
	case 22: sztext = _GetOrderWay(19); break;//'f' //跟单
	case 23: sztext = _GetOrderWay(20); break;//'t' //追单
	}

	return sztext;
}

/////////////////////////////////////////////////////////////////////////
char		szcommoditytypechs[][50] = {
	"现货", "现货延期", "期货",
	"跨期套利", "品种套利", "本地套利",
	"指数", "无效", "期权",
	"垂直看涨套利", "垂直看跌套利", "水平看涨套利", "水平看跌套利", "跨式套利", "宽跨式套利",
	"备兑组合", "外汇直接汇率", "外汇间接汇率", "外汇交叉汇率", "股票", "统计"
};
char		szcommoditytypecht[][50] = {
	"現貨", "現貨延期", "期貨",
	"跨期套利", "品種套利", "本地套利",
	"指數", "無效", "期權",
	"垂直看漲套利", "垂直看跌套利", "水平看漲套利", "水平看跌套利", "跨式套利", "寬跨式套利",
	"備兌組合", "外匯直接匯率", "外匯間接匯率", "外匯交叉匯率", "股票", "統計"
};
char		szcommoditytypeenu[][50] = {
	"Spot", "Spot Defer", "Futures",
	"Calendar Spread", "Commodity Spread", "Local Spread",
	"Index", "Invalid", "Options",
	"Vertical Call Spread", "Vertical Put Spread", "Horizontal Call Spreads", "Horizontal Put Spreads", "Straddle", "Strangle",
	"Covered Combination", "Direct Currency Exchange", "Indirect Currency Exchange", "Indirect Cross Exchange", "Stock", "Total"
};
char* _GetCommodityType(int i)
{
	if (g_language == ENU) return szcommoditytypeenu[i];
	else if (g_language == CHT) return szcommoditytypecht[i];
	return szcommoditytypechs[i];
}
void		TCommodityType2Char(TCommodityType ttype, char* sztext, int nsize)
{
	switch (ttype)
	{
	case ctUnDefine: strcpy_s(sztext, nsize, _GetOrdertype(0)); break;// sztext = _GetOrdertype(0); break;//			'U' //未定义

	case	ctGoods: strcpy_s(sztext, nsize, _GetCommodityType(0)); break;//sztext = _GetCommodityType(0); break;//				'G'	//现货
	case ctDefer: strcpy_s(sztext, nsize, _GetCommodityType(1)); break;//sztext = _GetCommodityType(1); break;//				'Y' //现货延期

	case	ctFutures: strcpy_s(sztext, nsize, _GetCommodityType(2)); break;//sztext = _GetCommodityType(2); break;//			'F'	//期货
	case ctSpreadMonth: strcpy_s(sztext, nsize, _GetCommodityType(3)); break;//sztext = _GetCommodityType(3); break;//		'm'	//跨期套利
	case ctSpreadCommodity: strcpy_s(sztext, nsize, _GetCommodityType(4)); break;//sztext = _GetCommodityType(4); break;//	'c'	//品种套利
	case ctSpreadLocal: strcpy_s(sztext, nsize, _GetCommodityType(5)); break;//sztext = _GetCommodityType(5); break;//		    'L'	//本地套利
	case ctIndexNumber: strcpy_s(sztext, nsize, _GetCommodityType(6)); break;//sztext = _GetCommodityType(6); break;//			'Z'	//指数
	case ctNone: strcpy_s(sztext, nsize, _GetCommodityType(7)); break;//sztext = _GetCommodityType(7); break;//		    'N'	//无效
	case	ctOption: strcpy_s(sztext, nsize, _GetCommodityType(8)); break;//sztext = _GetCommodityType(8); break;//		'O'	//期权
	case	ctBUL: strcpy_s(sztext, nsize, _GetCommodityType(9)); break;//sztext = _GetCommodityType(9); break;//				'u'	//垂直看涨套利
	case	ctBER: strcpy_s(sztext, nsize, _GetCommodityType(10)); break;//sztext = _GetCommodityType(10); break;//				'e'	//垂直看跌套利
	case	ctBLT: strcpy_s(sztext, nsize, _GetCommodityType(11)); break;//sztext = _GetCommodityType(11); break;//				'l'	//水平看涨套利	
	case ctBRT: strcpy_s(sztext, nsize, _GetCommodityType(12)); break;//sztext = _GetCommodityType(12); break;//				'r'	//水平看跌套利	
	case ctSTD: strcpy_s(sztext, nsize, _GetCommodityType(13)); break;//sztext = _GetCommodityType(13); break;//				'd'	//跨式套利
	case ctSTG: strcpy_s(sztext, nsize, _GetCommodityType(14)); break;//sztext = _GetCommodityType(14); break;//				'g'	//宽跨式套利
	case ctPRT: strcpy_s(sztext, nsize, _GetCommodityType(15)); break;//sztext = _GetCommodityType(15); break;//				'P' //备兑组合

	case ctDirect: strcpy_s(sztext, nsize, _GetCommodityType(16)); break;//sztext = _GetCommodityType(16); break;//			'D'	//外汇直接汇率
	case ctInDirect: strcpy_s(sztext, nsize, _GetCommodityType(17)); break;//sztext = _GetCommodityType(17); break;//				'I' //外汇间接汇率
	case ctCross: strcpy_s(sztext, nsize, _GetCommodityType(18)); break;//sztext = _GetCommodityType(18); break;//				'C' //外汇交叉汇率

	case ctStocks: strcpy_s(sztext, nsize, _GetCommodityType(19)); break;//sztext = _GetCommodityType(19); break;//			'T'	//股票
	case ctStatics: strcpy_s(sztext, nsize, _GetCommodityType(20)); break;//sztext = _GetCommodityType(20); break;//			'0'	//统计
	}
}

void		TErrorInfo2Char(int nerrorcode, char* errortext, char* sztext, int nsize)
{
	sprintf_s(sztext, nsize, "%d : %s", nerrorcode, errortext);
}

/////////////////////////////////////////////////////////////////////////
char		szstrategytypechs[][50] = {
	"埋单", "自动单", "条件单"
};
char		szstrategytypecht[][50] = {
	"埋單", "自動單", "條件單"
};
char		szstrategytypeenu[][50] = {
	"Pre-hour Order", "Auto Order", "Condition Order"
};
char* _GetStrategyType(int i)
{
	if (g_language == ENU) return szstrategytypeenu[i];
	else if (g_language == CHT) return szstrategytypecht[i];
	return szstrategytypechs[i];
}
void		TStrategyType2Char(TStrategyType& ttype, char* sztext, int nsize)
{
	switch (ttype)
	{
	case stNone: strcpy_s(sztext, nsize, _GetOrderState(0)); break;//sztext = _GetOrderState(0); break;//					'N' //无
	case stPreOrder: strcpy_s(sztext, nsize, _GetStrategyType(0)); break;//sztext = _GetStrategyType(0); break;//			'P' //预备单(埋单)
	case stAutoOrder: strcpy_s(sztext, nsize, _GetStrategyType(1)); break;//sztext = _GetStrategyType(1); break;//			'A' //自动单
	case stCondition: strcpy_s(sztext, nsize, _GetStrategyType(2)); break;//sztext = _GetStrategyType(2); break;//			'C' //条件单
	}
}

/////////////////////////////////////////////////////////////////////////
char		szboolchs[][50] = {
	"是", "否"
};
char		szboolcht[][50] = {
	"是", "否"
};
char		szboolenu[][50] = {
	"Yes", "No"
};
char* _GetBoolType(int i)
{
	if (g_language == ENU) return szboolenu[i];
	else if (g_language == CHT) return szboolcht[i];
	return szboolchs[i];
}
void		TBool2Char(TBool& tbool, char* sztext, int nsize)
{
	switch (tbool)
	{
	case	bYes: strcpy_s(sztext, nsize, _GetBoolType(0)); break;//sztext = _GetBoolType(0); break;//				'Y'	//是
	case	bNo: strcpy_s(sztext, nsize, _GetBoolType(1)); break;//sztext = _GetBoolType(1); break;//				'N'	//否
	}
}

/////////////////////////////////////////////////////////////////////////
char		szcheckmodechs[][50] = {
	"不审核", "自动审核", "人工审核",
	"待审核", "已通过", "未通过",
	"已在途", "自动止损", "自动止盈",
	"自动浮动", "保本", "未触发",
	"已触发", "已挂起", "已失效",
};
char		szcheckmodecht[][50] = {
	"不審核", "自動審核", "人工審核",
	"待審核", "已通過", "未通過",
	"已在途", "自動止損", "自動止盈",
	"自動浮動", "保本", "未觸發",
	"已觸發", "已掛起", "已失效",
};
char		szcheckmodeenu[][50] = {
	"Not Audit", "Auto review", "Manual Review",
	"Checking", "Passed", "Not Pass",
	"Float", "Auto Stop Loss", "Auto Stop Profit",
	"Auto Float", "Guaranteed", "Not trigger",
	"Triggered", "Suspended", "Failure",
};
char* _GetCheckModeType(int i)
{
	if (g_language == ENU) return szcheckmodeenu[i];
	else if (g_language == CHT) return szcheckmodecht[i];
	return szcheckmodechs[i];
}
void		TCheckMode2Char(TCheckMode tmode, char* sztext, int nsize)
{
	switch (tmode)
	{
	case cmNone: strcpy_s(sztext, nsize, _GetCheckModeType(0)); break;//sztext = _GetCheckModeType(0); break;//				'N' //不审核
	case cmAutoCheck: strcpy_s(sztext, nsize, _GetCheckModeType(1)); break;//sztext = _GetCheckModeType(1); break;//'A' //自动审核
	case cmManualCheck: strcpy_s(sztext, nsize, _GetCheckModeType(2)); break;//sztext = _GetCheckModeType(2); break;//'M' //人工审核
	}
}

void		TCheckState2Char(TCheckState tstate, char* sztext, int nsize)
{
	switch (tstate)
	{
	case csNone: strcpy_s(sztext, nsize, ""); break;//sztext = ""; break;//			'N'
	case csWaiting: strcpy_s(sztext, nsize, _GetCheckModeType(3)); break;//sztext = _GetCheckModeType(3); break;//			'W' //待审核
	case csPass: strcpy_s(sztext, nsize, _GetCheckModeType(4)); break;//sztext = _GetCheckModeType(4); break;//			'Y' //已通过
	case csFail: strcpy_s(sztext, nsize, _GetCheckModeType(5)); break;//sztext = _GetCheckModeType(5); break;//			'F' //未通过
	case csTransing: strcpy_s(sztext, nsize, _GetCheckModeType(6)); break;//sztext = _GetCheckModeType(6); break;//			'T' //已在途
	}
}

void		TStrokeTime2Char(std::string szsrc, int nbegin, int nlength, char* sztext, int nsize)
{
	if ((int)szsrc.length() < nbegin + nlength)return;

	strcpy_s(sztext, nsize, szsrc.substr(nbegin, nlength).c_str());
}

std::string strGetMarket(int i)
{
	std::string sztext;
	switch (i)
	{
	case 0:sztext = "CFFEX"; break;
	case 1:sztext = "DCE"; break;
	case 2:sztext = "SHFE"; break;
	case 3:sztext = "ZCE"; break;
	default:break;
	}
	return sztext;
}

std::string strGetTradeD(int i)
{
	std::string sztext;
	switch (i)
	{
	case 0:sztext = _GetDirect(0); break;
	case 1:sztext = _GetDirect(1); break;
	default:break;
	}
	return sztext;
}

std::string strGetLiquidate(int i)
{
	std::string sztext;
	switch (i)
	{
	case 0:sztext = _GetBoolType(0); break;
	case 1:sztext = _GetBoolType(1); break;
	default:break;
	}
	return sztext;
}

char szStopStateCHS[][20] = { "未触发", "已触发", "已挂起", "保本启动", "已删除" };
char szStopStateCHT[][20] = { "未觸發", "已觸發", "已掛起", "保本啟動", "已刪除" };
char szStopStateENU[][20] = { "Untriggered", "triggered", "Suspended", "Breakthrough", "Deleted" };

char* _GetStopState(int i)
{
	if (g_language == ENU) return szStopStateENU[i];
	else if (g_language == CHT) return szStopStateCHT[i];
	return szStopStateCHS[i];
}
void TStopState2Char(char orderState, char* szText, int Size)
{
	if (orderState == osGeneral)
		strcpy_s(szText, Size, _GetStopState(0));
	else if (orderState == osTrig)
		strcpy_s(szText, Size, _GetStopState(1));
	else if (orderState == osSuspend)
		strcpy_s(szText, Size, _GetStopState(2));
	else if (orderState == osBreak)
		strcpy_s(szText, Size, _GetStopState(3));
	else if (orderState == osDeleted)
		strcpy_s(szText, Size, _GetStopState(4));
	else
		strcpy_s(szText, Size, "");
}

char szStopOrderTypeCHS[][20] = { "止损单", "止盈单", "追踪回撤", "保本单" };
char szStopOrderTypeCHT[][20] = { "止損單", "止盈單", "追蹤回撤", "保本單" };
char szStopOrderTypeENU[][20] = { "StopLoss","StopProfit","FloatPoint","BreakEven" };

char* _GetStopOrderType(int i)
{
	if (g_language == ENU) return szStopOrderTypeENU[i];
	else if (g_language == CHT) return szStopOrderTypeCHT[i];
	return szStopOrderTypeCHS[i];
}

void TStopOrderType2Char(char orderType, char* szText, int Size)
{
	if (orderType == '0')
		strcpy_s(szText, Size, _GetStopOrderType(0));
	else if (orderType == '1')
		strcpy_s(szText, Size, _GetStopOrderType(1));
	else if (orderType == '2')
		strcpy_s(szText, Size, _GetStopOrderType(2));
	else if (orderType == '3')
		strcpy_s(szText, Size, _GetStopOrderType(3));
}
////////////////////////////////////////////////////////
char szPriceTypeChs[][20] = { "排队价", "对手价", "超价", "市价", "最新价" };
char szPriceTypeCht[][20] = { "排隊價", "對手價", "超價", "市價", "最新價" };
char szPriceTypeEnu[][20] = { "Queue Price", "Counter Price", "Super Price", "Market Price", "Last Price" };
char* _GetPriceType(int i)
{
	if (g_language == ENU) return szPriceTypeEnu[i];
	else if (g_language == CHT) return szPriceTypeCht[i];
	return szPriceTypeChs[i];
}
void TStopOrderPrice2Char(int iPriceType, char* szText, int Size)
{
	if (iPriceType == ptQueuePrice)
		strcpy_s(szText, Size, _GetPriceType(0));
	else if (iPriceType == ptCounterPrice)
		strcpy_s(szText, Size, _GetPriceType(1));
	else if (iPriceType == ptSuperPrice)
		strcpy_s(szText, Size, _GetPriceType(2));
	else if (iPriceType == ptMarketPrice)
		strcpy_s(szText, Size, _GetPriceType(3));
	else if (iPriceType == ptLastPrice)
		strcpy_s(szText, Size, _GetPriceType(4));
	else
		strcpy_s(szText, Size, "");
}

//////////////////////////////////////////////////////
char szConRunStateChs[][20] = { "运行中", "暂停", "已触发" };
char szConRunStateCht[][20] = { "運行中", "暫停", "已觸發" };
char szConRunStateEnu[][20] = { "Running", "Stop", "Triggered" };

char * _GetConRunState(int i)
{
	if (g_language == ENU) return szConRunStateEnu[i];
	else if (g_language == CHT) return szConRunStateCht[i];
	return szConRunStateChs[i];
}
void TConOrderState2Char(int state, char * szText, int length)
{
	if (state == rsRun)
		strcpy_s(szText, length, _GetConRunState(0));
	else if (state == rsStop)
		strcpy_s(szText, length, _GetConRunState(1));
	else if (state == rsTriggered)
		strcpy_s(szText, length, _GetConRunState(2));
}

//////////////////////////////////////////////////////////
char szConOrderTypeChs[][20] = { "本地价格条件单", "时间条件单", "预埋单", "自动单","服务器价格条件单" };
char szConOrderTypeCht[][20] = { "本地價格條件單", "時間條件單", "預埋單", "自動單", "服務器價格條件單" };
char szConOrderTypeEnu[][20] = { "Price", "Time", "Pre Order", "Auto Order","Server Price Cond." };

char * _GetConOrderType(int i)
{
	if (g_language == ENU) return szConOrderTypeEnu[i];
	else if (g_language == CHT) return szConOrderTypeCht[i];
	return szConOrderTypeChs[i];
}
void TConOrderType2Char(int type, char*szText, int length)
{
	switch (type)
	{
	case cotPrice:
		strcpy_s(szText, length, _GetConOrderType(0));
		break;
	case cotTime:
		strcpy_s(szText, length, _GetConOrderType(1));
		break;
	case cotPreOrder:
		strcpy_s(szText, length, _GetConOrderType(2));
		break;
	case cotAutoOrder:
		strcpy_s(szText, length, _GetConOrderType(3));
		break;
	case cotPriceServer:
		strcpy_s(szText, length, _GetConOrderType(4));
		break;
	default:
		strcpy_s(szText, length, "");
		break;
	}

}

UINT GetPriceType(wstring &wstrType)
{
	if (wstrType == LoadRC::ansi_to_unicode(_GetPriceType(0)))
		return ptQueuePrice;
	else if (wstrType == LoadRC::ansi_to_unicode(_GetPriceType(1)))
		return ptCounterPrice;
	else if (wstrType == LoadRC::ansi_to_unicode(_GetPriceType(2)))
		return ptSuperPrice;
	else if (wstrType == LoadRC::ansi_to_unicode(_GetPriceType(3)))
		return ptMarketPrice;
	else if (wstrType == LoadRC::ansi_to_unicode(_GetPriceType(4)))
		return ptLastPrice;
	return ptCounterPrice;
}
