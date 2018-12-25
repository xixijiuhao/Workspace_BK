#include "PreInclude.h"

//Ʒ������
typedef char               TapCommodityTypeType;
const TapCommodityTypeType		COMMODITY_TYPE_NONE = 'N';
//�ֻ�
const TapCommodityTypeType		COMMODITY_TYPE_SPOT = 'P';
//�ڻ�
const TapCommodityTypeType		COMMODITY_TYPE_FUTURES = 'F';
//��Ȩ
const TapCommodityTypeType		COMMODITY_TYPE_OPTION = 'O';
//��������
const TapCommodityTypeType		COMMODITY_TYPE_SPREAD_MONTH = 'S';
//��Ʒ������
const TapCommodityTypeType		COMMODITY_TYPE_SPREAD_COMMODITY = 'M';
//���Ǵ�ֱ����
const TapCommodityTypeType		COMMODITY_TYPE_BUL = 'U';
//������ֱ����
const TapCommodityTypeType		COMMODITY_TYPE_BER = 'E';
//��ʽ����
const TapCommodityTypeType		COMMODITY_TYPE_STD = 'D';
//���ʽ����
const TapCommodityTypeType		COMMODITY_TYPE_STG = 'G';
//�������
const TapCommodityTypeType		COMMODITY_TYPE_PRT = 'R';
//����ˮƽ��Ȩ
const TapCommodityTypeType		COMMODITY_TYPE_BLT = 'L';
//����ˮƽ��Ȩ
const TapCommodityTypeType		COMMODITY_TYPE_BRT = 'Q';
//��� ֱ�ӻ��� USD�ǻ�������
const TapCommodityTypeType		COMMODITY_TYPE_DIRECTFOREX = 'X';
//��� ֱ�ӻ���1 ��USD�ǻ�������
const TapCommodityTypeType		COMMODITY_TYPE_DIRECTFOREXR = 'Y';
//��� ��ӻ���
const TapCommodityTypeType		COMMODITY_TYPE_INDIRECTFOREX = 'I';
//��� �������
const TapCommodityTypeType		COMMODITY_TYPE_CROSSFOREX = 'C';
//ָ��
const TapCommodityTypeType		COMMODITY_TYPE_INDEX = 'Z';
//��Ʊ
const TapCommodityTypeType		COMMODITY_TYPE_STOCK = 'T';
//�н����������
const TapCommodityTypeType		COMMODITY_TYPE_FUTURE_LOCK = 'J';
//�н�����ͬ�·ݿ�Ʒ������
const TapCommodityTypeType		COMMODITY_TYPE_SPREAD_C_COMMODITY = 'K';
//����Ʒ������ת����
TCommodityType CConvert::TradeTypeToQuoteType(char cType)
{
	switch (cType)
	{
	case ctNone:				return COMMODITY_TYPE_NONE;
	case ctGoods:				return COMMODITY_TYPE_SPOT;
	case ctFutures:				return COMMODITY_TYPE_FUTURES;
	case ctOption:				return COMMODITY_TYPE_OPTION;
	case ctSpreadMonth:			return COMMODITY_TYPE_SPREAD_MONTH;
	case ctSpreadCommodity:		return COMMODITY_TYPE_SPREAD_COMMODITY;
	case ctBUL:					return COMMODITY_TYPE_BUL;
	case ctBER:					return COMMODITY_TYPE_BER;
	case ctSTD:					return COMMODITY_TYPE_STD;
	case ctSTG:					return COMMODITY_TYPE_STG;
	case ctPRT:					return COMMODITY_TYPE_PRT;
	case ctBLT:					return COMMODITY_TYPE_BLT;
	case ctBRT:					return COMMODITY_TYPE_BRT;
	case ctDirect:				return COMMODITY_TYPE_DIRECTFOREX;//COMMODITY_TYPE_DIRECTFOREXR
	case ctInDirect:			return COMMODITY_TYPE_INDIRECTFOREX;
	case ctCross:				return COMMODITY_TYPE_CROSSFOREX;
	case ctIndexNumber:			return COMMODITY_TYPE_INDEX;
	case ctStocks:				return COMMODITY_TYPE_STOCK;
	default:
		return cType;
	}
}
//���׺�ԼתΪ����ID
void CConvert::TradeContractToRawContractIdType(const TContractKey &ContractKey, char(&ContractId)[51])
{
	ContractId[0] = '\0';
	switch (ContractKey.CommodityType)
	{
		//�ڻ� <Exg>|<Type>|<Root>|<YEAR><MONTH>[DAY]
	case ctFutures:
		//ָ��
	case ctIndexNumber:
		sprintf_s(ContractId, "%s|%c|%s|%s",
			ContractKey.ExchangeNo,
			CConvert::TradeTypeToQuoteType(ContractKey.CommodityType),
			ContractKey.CommodityNo,
			ContractKey.ContractNo);
		break;
		//��Ȩ	<Exg>|<Type>|<Root>|<YEAR><MONTH>[DAY]<OFlag><STRIKEPRICE>
	case ctOption:
		sprintf_s(ContractId, "%s|%c|%s|%s%c%s",
			ContractKey.ExchangeNo,
			CConvert::TradeTypeToQuoteType(ContractKey.CommodityType),
			ContractKey.CommodityNo,
			ContractKey.ContractNo,
			ContractKey.OptionType,
			ContractKey.StrikePrice);
		break;
		//�������� <Exg>|<Type>|<Root>|<YEAR><MONTH>[DAY]|<YEAR><MONTH>[DAY]
	case ctSpreadMonth:
		sprintf_s(ContractId, "%s|%c|%s|%s|%s",
			ContractKey.ExchangeNo,
			CConvert::TradeTypeToQuoteType(ContractKey.CommodityType),
			ContractKey.CommodityNo,
			ContractKey.ContractNo,
			ContractKey.ContractNo2);
		break;
		//��Ʒ������ <Exg>|<Type>|<Root&Root>|<YEAR><MONTH>[DAY]
	case ctSpreadCommodity:
		sprintf_s(ContractId, "%s|%c|%s|%s",
			ContractKey.ExchangeNo,
			CConvert::TradeTypeToQuoteType(ContractKey.CommodityType),
			ContractKey.CommodityNo,
			ContractKey.ContractNo);
		break;
		//���Ǵ�ֱ����
	case ctBUL:
		//������ֱ����
	case ctBER:
		//��ʽ����
	case ctSTD:
		//���ʽ����
	case ctSTG:
		//��Ȩ����	<Exg>|<Type>|<Root[&Root]>|<YEAR><MONTH>[DAY]<OFlag><STRIKEPRICE>|<YEAR><MONTH>[DAY]<OFlag><STRIKEPRICE>
		sprintf_s(ContractId, "%s|%c|%s|%s%c%s|%s%c%s",
			ContractKey.ExchangeNo,
			CConvert::TradeTypeToQuoteType(ContractKey.CommodityType),
			ContractKey.CommodityNo,
			ContractKey.ContractNo,
			ContractKey.OptionType,
			ContractKey.StrikePrice,
			ContractKey.ContractNo2,
			ContractKey.OptionType2,
			ContractKey.StrikePrice2);
		break;
		//�������
	case ctPRT:
		break;
		//�ֻ�
	case ctGoods:
		//��㡪��ֱ�ӻ���
	case ctDirect:
		//��㡪����ӻ���
	case ctInDirect:
		//��㡪���������
	case ctCross:
		//��Ʊ
	case ctStocks:
	case ctStatics:
		sprintf_s(ContractId, "%s|%c|%s|",
			ContractKey.ExchangeNo,
			CConvert::TradeTypeToQuoteType(ContractKey.CommodityType),
			ContractKey.CommodityNo);
		break;
	default:
		sprintf_s(ContractId, "Unknown Contract:%c", CConvert::TradeTypeToQuoteType(ContractKey.CommodityType));
		break;
	}
}

TOffset CConvert::GetOffset(const char* UserNo, const TContractSingleKey * ContractKey, TDirect direct)
{
	uint LongCount = 0, ShortCount = 0, DayLongCount = 0, DayShortCount = 0;
	g_pTradeData->GetPositionVol(UserNo, ContractKey, LongCount, ShortCount, DayLongCount, DayShortCount);
	const TCommodity *pCommodity = g_pTradeData->GetCommodity(ContractKey);
	if (!pCommodity)
		return oNone;
	if (pCommodity->CoverMode == cmCoverToday)
	{
		if ((dBuy == direct&&DayShortCount > 0) || (dSell == direct&&DayLongCount > 0))
			return oCoverT;
		else if ((dBuy == direct&&ShortCount > 0) || (dSell == direct&&LongCount > 0))
			return oCover;
		else
			return oOpen;
	}
	else if (pCommodity->CoverMode == cmCover)
	{
		if ((dBuy == direct && ShortCount > 0) || (dSell == direct && LongCount > 0))
			return oCover;
		else
			return oOpen;
	}
	else
		return oNone;
}

void CConvert::DefaultSendOrder(TSendOrder&SendOrder)
{
	memset(&SendOrder, 0, sizeof(TSendOrder));

	SendOrder.CommodityType = ctNone;
	SendOrder.OptionType = otNone;
	SendOrder.OptionType2 = otNone;
	//SendOrder.OrderType = otUnDefine;
	//SendOrder.OrderWay = owUndefine;
	//SendOrder.ValidType = vtNone;
	/*SendOrder.IsRiskOrder = bNo;*/
	SendOrder.Direct = dNone;
	SendOrder.Offset = oNone;
	SendOrder.SellOffset = oNone;
	/*SendOrder.Hedge = hNone;*/
	SendOrder.TriggerMode = tmNone;
	SendOrder.TriggerCondition = tcNone;
	SendOrder.StrategyType = stNone;
	SendOrder.AddOneIsValid = tsDay;
	SendOrder.Hedge2 = hNone;
	//2016-2-22 10:32 xzc
	SendOrder.OrderType = otLimit;
	SendOrder.ValidType = vtGFD;
	SendOrder.OrderWay = owETrade;
	SendOrder.Hedge = hSpeculate;
	SendOrder.IsRiskOrder = bNo;
}

char		szorderstatechs[][50] = {
	"��", "�ѷ���", "������", "������", "��",
	"����Ч", "���Ŷ�", "���ֳɽ�", "��ȫ�ɽ�", "����",
	"����", "�ѳ���", "�ѳ��൥", "ָ��ʧ��", "�����",
	"�ѹ���", "������", "��Ч��", "���ִ���", "��ȫ����",
	"�൥ʧ��"
};

char* _GetOrderState(int i)
{
	return szorderstatechs[i];
}

void CConvert::TOrderState2Char(TOrderState tstate, TStrategyType ttype, char* sztext, int nsize/* = MAX_PATH*/)
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

char szdirectchs[][50] = {
	"����", "����", "˫��"
};
char* _GetDirect(int i)
{
	return szdirectchs[i];
}
void CConvert::TDirect2Char(TDirect tdirect, char* sztext, int nsize)
{
	switch (tdirect)
	{
	case dNone: strcpy_s(sztext, nsize, ""); break;// sztext = ""; break;//					'N'
	case	dBuy: strcpy_s(sztext, nsize, _GetDirect(0)); break; //sztext = _GetDirect(0); break;//				'B'	//����
	case	dSell: strcpy_s(sztext, nsize, _GetDirect(1)); break; //sztext = _GetDirect(1); break;//				'S'	//����
	case	dBoth: strcpy_s(sztext, nsize, _GetDirect(2)); break; //sztext = _GetDirect(2); break;//				'A'	//˫��
	}
}

char		szoffsetchs[][50] = {
	"����", "ƽ��", "ƽ��",
	"��ƽ", "ƽ��"
};
char* _GetOffset(int i)
{
	return szoffsetchs[i];
}
void CConvert::TOffset2Char(TOffset toffset, char* sztext, int nsize)
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

wstring CConvert::ANSIToUnicode(const string& str)//����AnsicתUnicode
{
	UINT nPage = 936;
	size_t  len = 0;
	len = str.length();
	int  unicodeLen = ::MultiByteToWideChar(nPage, 0, str.c_str(), -1, nullptr, 0);

	wchar_t *  pUnicode;
	pUnicode = new  wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1)*sizeof(wchar_t));

	::MultiByteToWideChar(nPage, 0, str.c_str(), -1, (LPWSTR)pUnicode, unicodeLen);
	wstring  rt = (wchar_t*)pUnicode;
	delete[]  pUnicode;

	return  rt;
}

COLORREF CConvert::SetOrderStateColor(TOrderState state)
{
	switch (state)
	{
	case osFilled:return RGB(0, 128, 0);
	case osQueued:return RGB(0, 0, 255);
	case osPartFilled:return RGB(0, 128, 128);
	case osPartCanceled:
	case osCanceled:return RGB(128, 128, 0); 
	case osSended:return RGB(128, 0, 128);
	case osFail:return RGB(255, 102, 0);
	case osAccept:return RGB(128, 0, 128); 
	default:return RGB(77, 96, 130);
	}
}
