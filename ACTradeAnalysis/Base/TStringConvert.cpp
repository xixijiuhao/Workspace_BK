#include "BaseInclude.h"

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
	"��", "�ѷ���", "������", "������", "��",
	"����Ч", "���Ŷ�", "���ֳɽ�", "��ȫ�ɽ�", "����",
	"����", "�ѳ���", "�ѳ��൥", "ָ��ʧ��", "�����",
	"�ѹ���", "������", "��Ч��", "���ִ���", "��ȫ����",
	"�൥ʧ��"
};
char		szorderstatecht[][50] = {
	"�o", "�Ѱl��", "������", "���|�l", "���",
	"����Ч", "�����", "���ֳɽ�", "��ȫ�ɽ�", "����",
	"����", "�ѳ���", "�ѳ�����", "ָ��ʧ��", "������",
	"�ђ���", "����Ո", "�oЧ��", "�����|�l", "��ȫ�|�l",
	"����ʧ��"
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
	case  osNone: strcpy_s(sztext, nsize, _GetOrderState(0)); break;//					'N' //��
	case	osSended: strcpy_s(sztext, nsize, _GetOrderState(1)); break;//				'0'	//�ѷ���
	case	osAccept: strcpy_s(sztext, nsize, _GetOrderState(2)); break;//				'1'	//������
	case osTriggering: //			'2'	//������
		if (ttype != stPreOrder)
			strcpy_s(sztext, nsize, _GetOrderState(3));
		else
			strcpy_s(sztext, nsize, _GetOrderState(4));
		break;
	case	osActive: strcpy_s(sztext, nsize, _GetOrderState(5)); break;//				'3'	//����Ч
	case	osQueued: strcpy_s(sztext, nsize, _GetOrderState(6)); break;//			'4'	//���Ŷ�
	case	osPartFilled: strcpy_s(sztext, nsize, _GetOrderState(7)); break;//			'5'	//���ֳɽ�
	case	osFilled: strcpy_s(sztext, nsize, _GetOrderState(8)); break;//			'6'	//��ȫ�ɽ�
	case	osCanceling: strcpy_s(sztext, nsize, _GetOrderState(9)); break;//				'7'	//����
	case	osModifying: strcpy_s(sztext, nsize, _GetOrderState(10)); break;//				'8'	//����
	case	osCanceled: strcpy_s(sztext, nsize, _GetOrderState(11)); break;//				'9'	//�ѳ���
	case	osPartCanceled: strcpy_s(sztext, nsize, _GetOrderState(12)); break;//				'A'	//�ѳ��൥
	case	osFail: strcpy_s(sztext, nsize, _GetOrderState(13)); break;//				'B'	//ָ��ʧ��
	case osChecking: strcpy_s(sztext, nsize, _GetOrderState(14)); break;//				'C' //�����
	case	osSuspended: strcpy_s(sztext, nsize, _GetOrderState(15)); break;//				'D'	//�ѹ���
	case	osApply: strcpy_s(sztext, nsize, _GetOrderState(16)); break;//					'E'	//������
	case osInvalid: strcpy_s(sztext, nsize, _GetOrderState(17)); break;//				'F' //��Ч��
	case osPartTriggered:strcpy_s(sztext, nsize, _GetOrderState(18)); break;//'G' //���ִ���
	case osFillTriggered:strcpy_s(sztext, nsize, _GetOrderState(19)); break;//			'H' //��ȫ����
	case osPartFailed:strcpy_s(sztext, nsize, _GetOrderState(20)); break;//'I' //�൥ʧ��
	}
}

std::string strGetOrderState(int i)
{
	std::string sztext;
	switch (i)
	{
	case 0: sztext = _GetOrderState(0); break;//					'N' //��
	case	1: sztext = _GetOrderState(1); break;//				'0'	//�ѷ���
	case	2: sztext = _GetOrderState(2); break;//				'1'	//������
	case 3: sztext = _GetOrderState(3); break;
	case	4: sztext = _GetOrderState(5); break;//				'3'	//����Ч
	case	5: sztext = _GetOrderState(6); break;//			'4'	//���Ŷ�
	case	6: sztext = _GetOrderState(7); break;//			'5'	//���ֳɽ�
	case	7: sztext = _GetOrderState(8); break;//			'6'	//��ȫ�ɽ�
	case	8: sztext = _GetOrderState(9); break;//				'7'	//����
	case	9: sztext = _GetOrderState(10); break;//				'8'	//����
	case	10: sztext = _GetOrderState(11); break;//				'9'	//�ѳ���
	case	11: sztext = _GetOrderState(12); break;//				'A'	//�ѳ��൥
	case	12: sztext = _GetOrderState(13); break;//				'B'	//ָ��ʧ��
	case 13: sztext = _GetOrderState(14); break;//				'C' //�����
	case	14: sztext = _GetOrderState(15); break;//				'D'	//�ѹ���
	case	15: sztext = _GetOrderState(16); break;//					'E'	//������
	case 16: sztext = _GetOrderState(17); break;//				'F' //��Ч��
	case 17: sztext = _GetOrderState(18); break;//				'F' //���ִ���
	case 18: sztext = _GetOrderState(19); break;//				'F' //��ȫ����
	case 19: sztext = _GetOrderState(20); break;//				'F' //�൥ʧ��

	default: sztext = "";
	}

	return sztext;
}

//////////////////////////////////////////////////////////////////////////
char		szdirectchs[][50] = {
	"����", "����", "˫��"
};
char		szdirectcht[][50] = {
	"�I��", "�u��", "�p߅"
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
	case	dBuy: strcpy_s(sztext, nsize, _GetDirect(0)); break; //sztext = _GetDirect(0); break;//				'B'	//����
	case	dSell: strcpy_s(sztext, nsize, _GetDirect(1)); break; //sztext = _GetDirect(1); break;//				'S'	//����
	case	dBoth: strcpy_s(sztext, nsize, _GetDirect(2)); break; //sztext = _GetDirect(2); break;//				'A'	//˫��
	}
}

//////////////////////////////////////////////////////////////////////////
char		szoffsetchs[][50] = {
	"����", "ƽ��", "ƽ��",
	"��ƽ", "ƽ��"
};
char		szoffsetcht[][50] = {
	"�_�}", "ƽ�}", "ƽ��",
	"�_ƽ", "ƽ�_"
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
	case	oNone: strcpy_s(sztext, nsize, _GetOrderState(0)); break;// sztext = _GetOrderState(0); break;//				'N'	//��
	case	oOpen: strcpy_s(sztext, nsize, _GetOffset(0)); break;// sztext = _GetOffset(0); break;//				'O'	//����
	case	oCover: strcpy_s(sztext, nsize, _GetOffset(1)); break;// sztext = _GetOffset(1); break;//				'C'	//ƽ��
	case	oCoverT: strcpy_s(sztext, nsize, _GetOffset(2)); break;//sztext = _GetOffset(2); break;//				'T'	//ƽ��
	case oOpenCover: strcpy_s(sztext, nsize, _GetOffset(3)); break;//sztext = _GetOffset(3); break;//				'1' //��ƽ��Ӧ��ʱ��Ч
	case oCoverOpen: strcpy_s(sztext, nsize, _GetOffset(4)); break;//sztext = _GetOffset(4); break;//				'2'	//ƽ����Ӧ��ʱ��Ч
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
	"δ����", "�м۵�", "�޼۵�",
	"�м�ֹ��", "�޼�ֹ��", "��Ȩ",
	"��Ȩ", "ѯ��", "Ӧ��",
	"��ɽ��", "Ӱ�ӵ�", "����",
	"��������", "�ױ�����"
};
char		szordertypecht[][50] = {
	"δ���x", "�Ѓr��", "�ރr��",
	"�Ѓrֹ�p", "�ރrֹ�p", "�Й�",
	"����", "ԃ�r", "���r",
	"��ɽ��", "Ӱ�ӆ�", "���Q",
	"������Ո", "�ױ���Ո"
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
	case otUnDefine: strcpy_s(sztext, nsize, _GetOrdertype(0)); break;// sztext = _GetOrdertype(0); break;//					'U' //δ����
	case	otMarket: strcpy_s(sztext, nsize, _GetOrdertype(1)); break;//sztext = _GetOrdertype(1); break;//				'1'	//�м۵�
	case	otLimit: strcpy_s(sztext, nsize, _GetOrdertype(2)); break;//sztext = _GetOrdertype(2); break;//					'2'	//�޼۵�
	case	otMarketStop: strcpy_s(sztext, nsize, _GetOrdertype(3)); break;//sztext = _GetOrdertype(3); break;//			'3'	//�м�ֹ��
	case	otLimitStop: strcpy_s(sztext, nsize, _GetOrdertype(4)); break;//sztext = _GetOrdertype(4); break;//				'4'	//�޼�ֹ��
	case otExecute: strcpy_s(sztext, nsize, _GetOrdertype(5)); break;//sztext = _GetOrdertype(5); break;//				'5' //��Ȩ
	case otAbandon: strcpy_s(sztext, nsize, _GetOrdertype(6)); break;//sztext = _GetOrdertype(6); break;//				'6' //��Ȩ
	case otEnquiry: strcpy_s(sztext, nsize, _GetOrdertype(7)); break;//sztext = _GetOrdertype(7); break;//				'7' //ѯ��
	case otOffer: strcpy_s(sztext, nsize, _GetOrdertype(8)); break;//sztext = _GetOrdertype(8); break;//				'8' //Ӧ��
	case otIceberg: strcpy_s(sztext, nsize, _GetOrdertype(9)); break;//sztext = _GetOrdertype(9); break;//				'9' //��ɽ��
	case otGhost: strcpy_s(sztext, nsize, _GetOrdertype(10)); break;//sztext = _GetOrdertype(10); break;//					'A' //Ӱ�ӵ�
	case otSwap: strcpy_s(sztext, nsize, _GetOrdertype(11)); break;//sztext = _GetOrdertype(11); break;//				'B' //����
	case otSpreadApply: strcpy_s(sztext, nsize, _GetOrdertype(12)); break;//sztext = _GetOrdertype(12); break;//			'C' //��������
	case otHedgApply: strcpy_s(sztext, nsize, _GetOrdertype(13)); break;//sztext = _GetOrdertype(13); break;//				'D' //�ױ�����
	}
}

//////////////////////////////////////////////////////////////////////////
char		sztriggermodechs[][50] = {
	"���¼�", "���", "����"
};
char		sztriggermodecht[][50] = {
	"���r", "�I�r", "�u�r"
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
	case tmNone: strcpy_s(sztext, nsize, _GetOrderState(0)); break;//sztext = _GetOrderState(0); break;//				'N' //��
	case tmLatest: strcpy_s(sztext, nsize, _GetTriggerMode(0)); break;//sztext = _GetTriggerMode(0); break;//		'L' //���¼�
	case tmBid: strcpy_s(sztext, nsize, _GetTriggerMode(1)); break;//sztext = _GetTriggerMode(1); break;//				'B' //���
	case tmAsk: strcpy_s(sztext, nsize, _GetTriggerMode(2)); break;//sztext = _GetTriggerMode(2); break;//				'A' //����
	}
}

/////////////////////////////////////////////////////////////////////////
char		sztriggerconditionchs[][50] = {
	"����", "���ڵ���", "С��",
	"С�ڵ���"
};
char		sztriggerconditioncht[][50] = {
	"���", "��춵��", "С�",
	"С춵��"
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
	case tcNone: strcpy_s(sztext, nsize, _GetOrderState(0)); break;// sztext = _GetOrderState(0); break;//				'N' //��
	case tcGreater: strcpy_s(sztext, nsize, _GetTriggerCondition(0)); break;//sztext = _GetTriggerCondition(0); break;//			'g' //����
	case tcGreaterEqual: strcpy_s(sztext, nsize, _GetTriggerCondition(1)); break;//sztext = _GetTriggerCondition(1); break;//		'G' //���ڵ���
	case tcLess: strcpy_s(sztext, nsize, _GetTriggerCondition(2)); break;//sztext = _GetTriggerCondition(2); break;//			'l' //С��
	case tcLessEqual: strcpy_s(sztext, nsize, _GetTriggerCondition(3)); break;//sztext = _GetTriggerCondition(3); break;//			'L' //С�ڵ��� }
	}
}

/////////////////////////////////////////////////////////////////////////
char		szhedgechs[][50] = {
	"Ͷ��", "�ױ�", "����",
	"����"
};
char		szhedgecht[][50] = {
	"Ͷ�C", "�ױ�", "����",
	"����"
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
	case	hNone: strcpy_s(sztext, nsize, _GetOrderState(0)); break;//sztext = _GetOrderState(0); break;//				'N'	//��
	case	hSpeculate: strcpy_s(sztext, nsize, _GetHedge(0)); break;//sztext = _GetHedge(0); break;//			'T'	//Ͷ��
	case	hHedge: strcpy_s(sztext, nsize, _GetHedge(1)); break;//sztext = _GetHedge(1); break;//				'B'	//�ױ�
	case hSpread: strcpy_s(sztext, nsize, _GetHedge(2)); break;//sztext = _GetHedge(2); break;//			'S' //����
	case hMarket: strcpy_s(sztext, nsize, _GetHedge(3)); break;//sztext = _GetHedge(3); break;//				'M' //����
	}
}

/////////////////////////////////////////////////////////////////////////
char		sztradesectionchs[][50] = {
	"����", "����(T+1)", "ȫ����"
};
char		sztradesectioncht[][50] = {
	"����", "����(T+1)", "ȫ����"
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
	case tsDay: strcpy_s(sztext, nsize, _GetTradeSection(0)); break;//sztext = _GetTradeSection(0); break;//				'D' //���콻��ʱ��
	case tsNight: strcpy_s(sztext, nsize, _GetTradeSection(1)); break;//sztext = _GetTradeSection(1); break;//			'N' //���ϣ�T+1������ʱ��
	case tsAll: strcpy_s(sztext, nsize, _GetTradeSection(2)); break;//sztext = _GetTradeSection(2); break;//			'A' //ȫ����ʱ��
	}
}

/////////////////////////////////////////////////////////////////////////
char		szvalidtypechs[][50] = {
	"��ʱȫ��", "��ʱ����", "������Ч",
	"������Ч", "������Ч", "������Ч"
};
char		szvalidtypecht[][50] = {
	"���rȫ��", "���r����", "������Ч",
	"������Ч", "�L����Ч", "������Ч"
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
	case vtNone: strcpy_s(sztext, nsize, _GetOrderState(0)); break;//sztext = _GetOrderState(0); break;//'N' //��
	case	vtFOK: strcpy_s(sztext, nsize, _GetValidType(0)); break;//sztext = _GetValidType(0); break;//'4'	//��ʱȫ��
	case	vtIOC: strcpy_s(sztext, nsize, _GetValidType(1)); break;//sztext = _GetValidType(1); break;//'3'	//��ʱ����
	case vtGFS: strcpy_s(sztext, nsize, _GetValidType(2)); break;//sztext = _GetValidType(2); break;//'5' //������Ч
	case	vtGFD: strcpy_s(sztext, nsize, _GetValidType(3)); break;//sztext = _GetValidType(3); break;//'0'	//������Ч
	case	vtGTC: strcpy_s(sztext, nsize, _GetValidType(4)); break;//sztext = _GetValidType(4); break;//'1'	//������Ч
	case	vtGTD: strcpy_s(sztext, nsize, _GetValidType(5)); break;//sztext = _GetValidType(5);; break;//'2'	//������Ч
	}
}

/////////////////////////////////////////////////////////////////////////
char		szorderwaychs[][50] = {
	"����", "����", "�˹���",
	"Carry��", "������Ȩ", "��ʽ��",
	"��Ȩ", "��Ȩ", "ͨ����",
	"RTS", "��������", "�����µ�", "�����µ�",
	"����µ�", "��Ƭ�µ�", "�����µ�", "�����µ�",
	"ƹ��", "ֹ��ֹӯ", "����", "׷��"
};
char		szorderwaycht[][50] = {
	"����", "�����", "�˹���",
	"Carry��", "�����Й�", "��ʽ��",
	"�Й�", "����", "ͨ���M",
	"RTS", "��������", "�M����", "�Q����",
	"�c�r��", "��Ƭ��", "������", "������",
	"ƹ��", "ֹ�pֹӯ", "����", "׷��"
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
	case owUndefine: strcpy_s(sztext, nsize, _GetOrdertype(0)); break;//sztext = _GetOrdertype(0); break;//				'U' //δ����
	case	owAll: strcpy_s(sztext, nsize, _GetOrderWay(0)); break;//sztext = _GetOrderWay(0); break;//				'A'	//����
	case	owETrade: strcpy_s(sztext, nsize, "E-Trade"); break;//sztext = "E-Trade"; break;//		'E'	//E-Trade
	case	owProxyETrade: strcpy_s(sztext, nsize, _GetOrderWay(1)); break;//sztext = _GetOrderWay(1); break;//			'P'	//����
	case	owJTrade: strcpy_s(sztext, nsize, "J-Trade"); break;//sztext = "J-Trade"; break;//			'J'	//J-Trade
	case	owManual: strcpy_s(sztext, nsize, _GetOrderWay(2)); break;//sztext = _GetOrderWay(2); break;//			'M'	//�˹���
	case	owCarry: strcpy_s(sztext, nsize, _GetOrderWay(3)); break;//sztext = _GetOrderWay(3); break;//				'C'	//Carry��
	case	owDelivery: strcpy_s(sztext, nsize, _GetOrderWay(4)); break;//sztext = _GetOrderWay(4); break;//			'D'	//������Ȩ
	case	owProgram: strcpy_s(sztext, nsize, _GetOrderWay(5)); break;//sztext = _GetOrderWay(5); break;//				'S' //��ʽ��	
	case owExecute: strcpy_s(sztext, nsize, _GetOrderWay(6)); break;//sztext = _GetOrderWay(6); break;//				'e' //��Ȩ
	case owAbandon: strcpy_s(sztext, nsize, _GetOrderWay(7)); break;//sztext = _GetOrderWay(7); break;//			'a' //��Ȩ
	case owChannel: strcpy_s(sztext, nsize, _GetOrderWay(8)); break;//sztext = _GetOrderWay(8); break;//			'c' //ͨ����

	case owRTS: strcpy_s(sztext, nsize, _GetOrderWay(9)); break;//sztext = _GetOrderWay(9); break;//'R' //RTS
//	case owSpreadLocal: strcpy_s(sztext, nsize, _GetOrderWay(10)); break;//sztext = _GetOrderWay(10); break;//'L' //��������
	case	owHorz: strcpy_s(sztext, nsize, _GetOrderWay(11)); break;//sztext = _GetOrderWay(11); break;//'H'	//�����µ�
	case	owVert: strcpy_s(sztext, nsize, _GetOrderWay(12)); break;//sztext = _GetOrderWay(12); break;//'V'	//�����µ�
	case	owClickPrice: strcpy_s(sztext, nsize, _GetOrderWay(13)); break;//sztext = _GetOrderWay(13); break;//'l'	//����µ�
	case	owCard: strcpy_s(sztext, nsize, _GetOrderWay(14)); break;//sztext = _GetOrderWay(14); break;//'r' //��Ƭ�µ�	
	case owBrtch: strcpy_s(sztext, nsize, _GetOrderWay(15)); break;//sztext = _GetOrderWay(15); break;//'B' //�����µ�
	case owFast: strcpy_s(sztext, nsize, _GetOrderWay(16)); break;//sztext = _GetOrderWay(16); break;//'F' //�����µ�
	case owPingPong: strcpy_s(sztext, nsize, _GetOrderWay(17)); break;//sztext = _GetOrderWay(17); break;//'p' //ƹ��
//	case	owStop: strcpy_s(sztext, nsize, _GetOrderWay(18)); break;//sztext = _GetOrderWay(18); break;//'s' //ֹ��ֹӯ
	case owFollow: strcpy_s(sztext, nsize, _GetOrderWay(19)); break;//sztext = _GetOrderWay(19); break;//'f' //����
	case owCatch: strcpy_s(sztext, nsize, _GetOrderWay(20)); break;//sztext = _GetOrderWay(20); break;//'t' //׷��
	}
}

std::string strGetOrderWay(int i)
{
	std::string sztext;
	switch (i)
	{
	case 0: sztext = _GetOrdertype(0); break;//				'U' //δ����
	case	1: sztext = _GetOrderWay(0); break;//				'A'	//����
	case	2: sztext = "E-Trade"; break;//		'E'	//E-Trade
	case	3: sztext = _GetOrderWay(1); break;//			'P'	//����
	case	4: sztext = "J-Trade"; break;//			'J'	//J-Trade
	case	5: sztext = _GetOrderWay(2); break;//			'M'	//�˹���
	case	6: sztext = _GetOrderWay(3); break;//				'C'	//Carry��
	case	7: sztext = _GetOrderWay(4); break;//			'D'	//������Ȩ
	case	8: sztext = _GetOrderWay(5); break;//				'S' //��ʽ��	
	case 9: sztext = _GetOrderWay(6); break;//				'e' //��Ȩ
	case 10: sztext = _GetOrderWay(7); break;//			'a' //��Ȩ
	case 11: sztext = _GetOrderWay(8); break;//			'c' //ͨ����

	case 12: sztext = _GetOrderWay(9); break;//'R' //RTS
	case 13: sztext = _GetOrderWay(10); break;//'L' //��������
	case	14: sztext = _GetOrderWay(11); break;//'H'	//�����µ�
	case	15: sztext = _GetOrderWay(12); break;//'V'	//�����µ�
	case	16: sztext = _GetOrderWay(13); break;//'l'	//����µ�
	case	17: sztext = _GetOrderWay(14); break;//'r' //��Ƭ�µ�	
	case 18: sztext = _GetOrderWay(15); break;//'B' //�����µ�
	case 19: sztext = _GetOrderWay(16); break;//'F' //�����µ�
	case 20: sztext = _GetOrderWay(17); break;//'p' //ƹ��
	case	21: sztext = _GetOrderWay(18); break;//'s' //ֹ��ֹӯ
	case 22: sztext = _GetOrderWay(19); break;//'f' //����
	case 23: sztext = _GetOrderWay(20); break;//'t' //׷��
	}

	return sztext;
}

/////////////////////////////////////////////////////////////////////////
char		szcommoditytypechs[][50] = {
	"�ֻ�", "�ֻ�����", "�ڻ�",
	"��������", "Ʒ������", "��������",
	"ָ��", "��Ч", "��Ȩ",
	"��ֱ��������", "��ֱ��������", "ˮƽ��������", "ˮƽ��������", "��ʽ����", "���ʽ����",
	"�������", "���ֱ�ӻ���", "����ӻ���", "��㽻�����", "��Ʊ", "ͳ��"
};
char		szcommoditytypecht[][50] = {
	"�F؛", "�F؛����", "��؛",
	"��������", "Ʒ�N����", "��������",
	"ָ��", "�oЧ", "�ڙ�",
	"��ֱ���q����", "��ֱ��������", "ˮƽ���q����", "ˮƽ��������", "��ʽ����", "����ʽ����",
	"�䃶�M��", "��Rֱ�ӅR��", "��R�g�ӅR��", "��R����R��", "��Ʊ", "�yӋ"
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
	case ctUnDefine: strcpy_s(sztext, nsize, _GetOrdertype(0)); break;// sztext = _GetOrdertype(0); break;//			'U' //δ����

	case	ctGoods: strcpy_s(sztext, nsize, _GetCommodityType(0)); break;//sztext = _GetCommodityType(0); break;//				'G'	//�ֻ�
	case ctDefer: strcpy_s(sztext, nsize, _GetCommodityType(1)); break;//sztext = _GetCommodityType(1); break;//				'Y' //�ֻ�����

	case	ctFutures: strcpy_s(sztext, nsize, _GetCommodityType(2)); break;//sztext = _GetCommodityType(2); break;//			'F'	//�ڻ�
	case ctSpreadMonth: strcpy_s(sztext, nsize, _GetCommodityType(3)); break;//sztext = _GetCommodityType(3); break;//		'm'	//��������
	case ctSpreadCommodity: strcpy_s(sztext, nsize, _GetCommodityType(4)); break;//sztext = _GetCommodityType(4); break;//	'c'	//Ʒ������
	case ctSpreadLocal: strcpy_s(sztext, nsize, _GetCommodityType(5)); break;//sztext = _GetCommodityType(5); break;//		    'L'	//��������
	case ctIndexNumber: strcpy_s(sztext, nsize, _GetCommodityType(6)); break;//sztext = _GetCommodityType(6); break;//			'Z'	//ָ��
	case ctNone: strcpy_s(sztext, nsize, _GetCommodityType(7)); break;//sztext = _GetCommodityType(7); break;//		    'N'	//��Ч
	case	ctOption: strcpy_s(sztext, nsize, _GetCommodityType(8)); break;//sztext = _GetCommodityType(8); break;//		'O'	//��Ȩ
	case	ctBUL: strcpy_s(sztext, nsize, _GetCommodityType(9)); break;//sztext = _GetCommodityType(9); break;//				'u'	//��ֱ��������
	case	ctBER: strcpy_s(sztext, nsize, _GetCommodityType(10)); break;//sztext = _GetCommodityType(10); break;//				'e'	//��ֱ��������
	case	ctBLT: strcpy_s(sztext, nsize, _GetCommodityType(11)); break;//sztext = _GetCommodityType(11); break;//				'l'	//ˮƽ��������	
	case ctBRT: strcpy_s(sztext, nsize, _GetCommodityType(12)); break;//sztext = _GetCommodityType(12); break;//				'r'	//ˮƽ��������	
	case ctSTD: strcpy_s(sztext, nsize, _GetCommodityType(13)); break;//sztext = _GetCommodityType(13); break;//				'd'	//��ʽ����
	case ctSTG: strcpy_s(sztext, nsize, _GetCommodityType(14)); break;//sztext = _GetCommodityType(14); break;//				'g'	//���ʽ����
	case ctPRT: strcpy_s(sztext, nsize, _GetCommodityType(15)); break;//sztext = _GetCommodityType(15); break;//				'P' //�������

	case ctDirect: strcpy_s(sztext, nsize, _GetCommodityType(16)); break;//sztext = _GetCommodityType(16); break;//			'D'	//���ֱ�ӻ���
	case ctInDirect: strcpy_s(sztext, nsize, _GetCommodityType(17)); break;//sztext = _GetCommodityType(17); break;//				'I' //����ӻ���
	case ctCross: strcpy_s(sztext, nsize, _GetCommodityType(18)); break;//sztext = _GetCommodityType(18); break;//				'C' //��㽻�����

	case ctStocks: strcpy_s(sztext, nsize, _GetCommodityType(19)); break;//sztext = _GetCommodityType(19); break;//			'T'	//��Ʊ
	case ctStatics: strcpy_s(sztext, nsize, _GetCommodityType(20)); break;//sztext = _GetCommodityType(20); break;//			'0'	//ͳ��
	}
}

void		TErrorInfo2Char(int nerrorcode, char* errortext, char* sztext, int nsize)
{
	sprintf_s(sztext, nsize, "%d : %s", nerrorcode, errortext);
}

/////////////////////////////////////////////////////////////////////////
char		szstrategytypechs[][50] = {
	"��", "�Զ���", "������"
};
char		szstrategytypecht[][50] = {
	"���", "�Ԅӆ�", "�l����"
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
	case stNone: strcpy_s(sztext, nsize, _GetOrderState(0)); break;//sztext = _GetOrderState(0); break;//					'N' //��
	case stPreOrder: strcpy_s(sztext, nsize, _GetStrategyType(0)); break;//sztext = _GetStrategyType(0); break;//			'P' //Ԥ����(��)
	case stAutoOrder: strcpy_s(sztext, nsize, _GetStrategyType(1)); break;//sztext = _GetStrategyType(1); break;//			'A' //�Զ���
	case stCondition: strcpy_s(sztext, nsize, _GetStrategyType(2)); break;//sztext = _GetStrategyType(2); break;//			'C' //������
	}
}

/////////////////////////////////////////////////////////////////////////
char		szboolchs[][50] = {
	"��", "��"
};
char		szboolcht[][50] = {
	"��", "��"
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
	case	bYes: strcpy_s(sztext, nsize, _GetBoolType(0)); break;//sztext = _GetBoolType(0); break;//				'Y'	//��
	case	bNo: strcpy_s(sztext, nsize, _GetBoolType(1)); break;//sztext = _GetBoolType(1); break;//				'N'	//��
	}
}

/////////////////////////////////////////////////////////////////////////
char		szcheckmodechs[][50] = {
	"�����", "�Զ����", "�˹����",
	"�����", "��ͨ��", "δͨ��",
	"����;", "�Զ�ֹ��", "�Զ�ֹӯ",
	"�Զ�����", "����", "δ����",
	"�Ѵ���", "�ѹ���", "��ʧЧ",
};
char		szcheckmodecht[][50] = {
	"������", "�Ԅӌ���", "�˹�����",
	"������", "��ͨ�^", "δͨ�^",
	"����;", "�Ԅ�ֹ�p", "�Ԅ�ֹӯ",
	"�ԄӸ���", "����", "δ�|�l",
	"���|�l", "�ђ���", "��ʧЧ",
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
	case cmNone: strcpy_s(sztext, nsize, _GetCheckModeType(0)); break;//sztext = _GetCheckModeType(0); break;//				'N' //�����
	case cmAutoCheck: strcpy_s(sztext, nsize, _GetCheckModeType(1)); break;//sztext = _GetCheckModeType(1); break;//'A' //�Զ����
	case cmManualCheck: strcpy_s(sztext, nsize, _GetCheckModeType(2)); break;//sztext = _GetCheckModeType(2); break;//'M' //�˹����
	}
}

void		TCheckState2Char(TCheckState tstate, char* sztext, int nsize)
{
	switch (tstate)
	{
	case csNone: strcpy_s(sztext, nsize, ""); break;//sztext = ""; break;//			'N'
	case csWaiting: strcpy_s(sztext, nsize, _GetCheckModeType(3)); break;//sztext = _GetCheckModeType(3); break;//			'W' //�����
	case csPass: strcpy_s(sztext, nsize, _GetCheckModeType(4)); break;//sztext = _GetCheckModeType(4); break;//			'Y' //��ͨ��
	case csFail: strcpy_s(sztext, nsize, _GetCheckModeType(5)); break;//sztext = _GetCheckModeType(5); break;//			'F' //δͨ��
	case csTransing: strcpy_s(sztext, nsize, _GetCheckModeType(6)); break;//sztext = _GetCheckModeType(6); break;//			'T' //����;
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



////////////////////////////////////////////////////////
char szPriceTypeChs[][20] = { "�ŶӼ�", "���ּ�", "����", "�м�", "���¼�" };
char szPriceTypeCht[][20] = { "��ꠃr", "���փr", "���r", "�Ѓr", "���r" };
char szPriceTypeEnu[][20] = { "Queue Price", "Counter Price", "Super Price", "Market Price", "Last Price" };
char* _GetPriceType(int i)
{
	if (g_language == ENU) return szPriceTypeEnu[i];
	else if (g_language == CHT) return szPriceTypeCht[i];
	return szPriceTypeChs[i];
}
void TStopOrderPrice2Char(int iPriceType, char* szText, int Size)
{
}




UINT GetPriceType(wstring &wstrType)
{
	return 1;
}
