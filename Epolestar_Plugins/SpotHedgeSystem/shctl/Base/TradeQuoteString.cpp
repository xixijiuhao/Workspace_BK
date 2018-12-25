//#include "TradeQuoteString.h"
#include "PreInclude.h"

using namespace std;

extern ITradeData* g_pTradeData;
extern IStarApi * g_pStarApi;
extern ILanguageApi * g_pLanguage;
extern ITradeApi *g_pTradeApi;
extern ITradeData *g_pTradeData;


void CConvert::DefaultSendOrder(TSendOrder&SendOrder)
{
	memset(&SendOrder, 0, sizeof(TSendOrder));

	SendOrder.CommodityType = ctNone;
	SendOrder.OptionType = otNone;
	SendOrder.OptionType2 = otNone;
	SendOrder.OrderType = otUnDefine;
	SendOrder.OrderWay = owUndefine;
	SendOrder.ValidType = vtNone;
	SendOrder.IsRiskOrder = bNo;
	SendOrder.Direct = dNone;
	SendOrder.Offset = oNone;
	SendOrder.SellOffset = oNone;
	SendOrder.Hedge = hNone;
	SendOrder.TriggerMode = tmNone;
	SendOrder.TriggerCondition = tcNone;
	SendOrder.StrategyType = stNone;
	SendOrder.AddOneIsValid = tsDay;
	SendOrder.SellHedge = hNone;
}

//����Ƿ��н���Ա��¼��
bool CConvert::IsTraderLogined()
{
	TDataContainer tempUser;
	g_pTradeData->GetAllLogin(tempUser);
	for (uint i = 0; i < tempUser.size(); i++)
	{
		const TLogin *pLogin = ((TLogin*)(tempUser.at(i)));
		if (pLogin && (pLogin->Logined == bYes) && (uiTrader == pLogin->UserType) && pLogin->ReadyCount>0)
			return true;
	}
	return false;
}

//LME�ж�
bool CConvert::IsLME(const TExchangeKey &Exchange)
{ 
	return strcmp(Exchange.ExchangeNo,"LME")==0;
}

//�ַ���ת���׺�Լ
bool CConvert::TContractNoToTContractKey(const char * szCode, TContractKey & ContractKey)//��֤���������ַ����������˶�ȥ���ո���
{
	vector<string> vecstr;		//�������������
	string tempstr(szCode);
	string::iterator tempit = tempstr.begin();
	while (tempit != tempstr.end())
	{
		tempit = find<string::iterator, char>(tempstr.begin(), tempstr.end(), ' ');
		if (tempit != tempstr.end())
		{
			iterator_traits<string::iterator>::value_type dis = distance(tempstr.begin(), tempit);
			vecstr.push_back(tempstr.substr(0, dis));
			if (++tempit != tempstr.end())
			{
				tempstr.erase(tempstr.begin(), tempit);
				tempit = tempstr.begin();
			}
			else	break;
		}
		else//û�ҵ�' '��������  ����abc;������ȻҪ����
		{
			iterator_traits<string::iterator>::value_type dis = distance(tempit, tempstr.end());
			vecstr.push_back(tempstr);
			break;
		}
	}
	vector<string>::size_type iCount = vecstr.size();
	if (iCount < 2 || iCount>5)	return false;
	//��ֹ�����������ո�
	for (vector<string>::size_type i = 0; i < iCount; i++)
	{
		if (vecstr[i].empty())	return false;
	}
	strncpy_s(ContractKey.ExchangeNo, sizeof(ContractKey.ExchangeNo), vecstr[0].c_str(), _TRUNCATE);					//������
	strncpy_s(ContractKey.CommodityNo, sizeof(ContractKey.CommodityNo), vecstr[1].c_str(), _TRUNCATE);				//��Ʒ
	ContractKey.OptionType = otNone;
	ContractKey.OptionType2 = otNone;
	string str;
	if (iCount == 2)
	{
		if (vecstr[0]==exSGE)
		{
			ContractKey.CommodityType = ctGoods;
			int iPs = vecstr[1].find('(');
			if (string::npos != iPs)	ContractKey.CommodityType = ctDefer;
		}
		else
		{
			if (strncmp(vecstr[1].c_str(), "USD", 3) == 0)
				ContractKey.CommodityType = ctDirect;
			else if (strstr(vecstr[1].c_str(), "USD"))
				ContractKey.CommodityType = ctInDirect;
			else
				ContractKey.CommodityType = ctCross;
		}
	}
	else if (iCount == 3)														//�ڻ������ڻ����
	{
		int iPos2 = vecstr[1].find('&');
		if (string::npos != iPos2)	//��Ʒ��
		{
			ContractKey.CommodityType = ctSpreadCommodity;
			strncpy_s(ContractKey.ContractNo, sizeof(ContractKey.ContractNo), vecstr[2].c_str(), _TRUNCATE);
			strncpy_s(ContractKey.ContractNo2, sizeof(ContractKey.ContractNo2), vecstr[2].c_str(), _TRUNCATE);
			return true;
		}
		int isubPos = vecstr[2].find('/');
		if (string::npos != isubPos)										//���
		{
			if (++isubPos == string::npos)		return false;
			ContractKey.CommodityType = ctSpreadMonth;
			str = vecstr[2].substr(0, --isubPos);
			strncpy_s(ContractKey.ContractNo, sizeof(ContractKey.ContractNo), str.c_str(), _TRUNCATE);
			strncpy_s(ContractKey.ContractNo2, sizeof(ContractKey.ContractNo2), vecstr[2].substr(++isubPos, vecstr[2].size() - isubPos).c_str(), _TRUNCATE);
		}
		else															//����
		{
			ContractKey.CommodityType = ctFutures;
			strncpy_s(ContractKey.ContractNo, sizeof(ContractKey.ContractNo), vecstr[2].c_str(), _TRUNCATE);
		}
	}
	else if (4 == iCount)													//��Ȩ
	{
		ContractKey.CommodityType = ctOption;
		strncpy_s(ContractKey.ContractNo, sizeof(ContractKey.ContractNo), vecstr[2].c_str(), _TRUNCATE);
		if (otCall == vecstr[3].at(0))									//����
			ContractKey.OptionType = otCall;
		else if (otPut == vecstr[3].at(0))									//����
			ContractKey.OptionType = otPut;
		strncpy_s(ContractKey.StrikePrice, sizeof(ContractKey.StrikePrice), vecstr[3].substr(1).c_str(), _TRUNCATE);
	}
	else if (5 == iCount)													//��Ȩ��� ĿǰΪ���� ���ҵ�һ�� �ڶ������ϸ��˳��
	{
		strncpy_s(ContractKey.ContractNo, sizeof(ContractKey.ContractNo), vecstr[2].c_str(), _TRUNCATE);
		if (otCall == vecstr[3].at(0))									//���� ��һ��
			ContractKey.OptionType = otCall;
		else if (otPut == vecstr[3].at(0))									//����
			ContractKey.OptionType = otPut;
		vecstr[3] = vecstr[3].substr(1);
		int isubPos = vecstr[3].find('/');
		if (string::npos != isubPos)										//���
		{
			if (++isubPos == string::npos)
				return false;
			str = vecstr[3].substr(0, --isubPos);							//
			strncpy_s(ContractKey.StrikePrice, sizeof(ContractKey.StrikePrice), str.c_str(), _TRUNCATE);				//4300/307
			strncpy_s(ContractKey.ContractNo2, sizeof(ContractKey.ContractNo2), vecstr[3].substr(++isubPos, vecstr[3].size() - isubPos).c_str(), _TRUNCATE);
		}

		if (otCall == vecstr[4].at(0))									//���� �ڶ���
			ContractKey.OptionType2 = otCall;
		else if (otPut == vecstr[4].at(0))									//����
			ContractKey.OptionType2 = otPut;
		strncpy_s(ContractKey.StrikePrice2, sizeof(ContractKey.StrikePrice2), vecstr[4].substr(1).c_str(), _TRUNCATE);

		//�ж�Ʒ������
		if (0 == strcmp(ContractKey.ContractNo, ContractKey.ContractNo2))	//��Լ�·���ͬ
		{
			if (ContractKey.OptionType == ContractKey.OptionType2)			//ͬ���ǻ�ͬ����
			{
				if ((strcmp(ContractKey.StrikePrice, ContractKey.StrikePrice2) > 0) &&
					(otPut == ContractKey.OptionType))					//ִ�м�1>ִ�м�2 ͬ���� BER 4-������Ȩ��ֱ�۲�
				{
					ContractKey.CommodityType = ctBER;
				}
				else if ((strcmp(ContractKey.StrikePrice, ContractKey.StrikePrice2) < 0) &&
					(otCall == ContractKey.OptionType))			 		///ִ�м�1<ִ�м�2 ͬ���� BUL 3-������Ȩ��ֱ�۲� 
				{
					ContractKey.CommodityType = ctBUL;
				}
				else	return false;
			}
			else														//���ǿ������߿�������
			{
				if ((otCall == ContractKey.OptionType) &&
					(otPut == ContractKey.OptionType2))					//��һ�ȿ��� �ڶ��ȿ���
				{
					if (0 == strcmp(ContractKey.StrikePrice, ContractKey.StrikePrice2))
						ContractKey.CommodityType = ctSTD;				//STD 7-��ʽ���
					else if (strcmp(ContractKey.StrikePrice, ContractKey.StrikePrice2) > 0)
						ContractKey.CommodityType = ctSTG;				//STG 8-����ʽ���
				}
				else	return false;
			}
		}
		else															//BLT 5-������Ȩˮƽ�۲� BRT 6-������Ȩˮƽ�۲�
		{
			if ((0 == strcmp(ContractKey.StrikePrice, ContractKey.StrikePrice2)) &&
				(ContractKey.OptionType == ContractKey.OptionType2))		//�·ݲ�ͬ��ִ�м���ͬ��ͬ���ǻ���ͬ����
			{
				if (otPut == ContractKey.OptionType)						//
					ContractKey.CommodityType = ctBRT;
				else if (otCall == ContractKey.OptionType)
					ContractKey.CommodityType = ctBLT;
				else	return false;
			}
			else		return false;
		}
	}
	return true;
}


//���׺�Լת�ַ��� bOnlyContractΪtrueʱֻ��Ҫ��ʽ����Լ���� ����Ҫ����������Ʒ
bool CConvert::TContractKeyToTContractNo(const TContractKey & ContractKey, string & strCode)//
{
	char strText[100] = "";
	if (ContractKey.CommodityType == ctFutures)//�ڻ�,�ֻ����ֻ�����
	{
		sprintf_s(strText, "%s %s %s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo);	
	}
	else if (ContractKey.CommodityType == ctGoods || ContractKey.CommodityType == ctDefer || ContractKey.CommodityType == ctDirect || ContractKey.CommodityType == ctInDirect || ContractKey.CommodityType == ctCross)
	{
		sprintf_s(strText, "%s %s", ContractKey.ExchangeNo, ContractKey.CommodityNo);
	}
	else if (ContractKey.CommodityType == ctOption)//��Ȩ
	{
		sprintf_s(strText, "%s %s %s %c%s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice);		
	}
	else if (ContractKey.CommodityType == ctSpreadMonth)//�ڻ��� ��������
	{
		sprintf_s(strText, "%s %s %s/%s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.ContractNo2);	
	}
	else if (ContractKey.CommodityType == ctSpreadCommodity)//����Ʒ����
	{
		sprintf_s(strText, "%s %s %s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo);	
	}
	else if (ContractKey.CommodityType == ctBUL || ContractKey.CommodityType == ctBER || ContractKey.CommodityType == ctBLT || ContractKey.CommodityType == ctBRT || ContractKey.CommodityType == ctSTD || ContractKey.CommodityType == ctSTG)//��Ȩ�� ������Ȩ��ֱ�۲������Ȩ��ֱ�۲��ʽ��ϣ�����ʽ��ϣ�ˮƽ����������ˮƽ��������	
	{
		sprintf_s(strText, "%s %s %s %c%s/%s %c%s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice, ContractKey.ContractNo2, ContractKey.OptionType2, ContractKey.StrikePrice2);			
	}
	strCode = strText;
	return true;
}
bool CConvert::TContractKeyToTContractNo2(const TContractKey & ContractKey, std::string & strCode, bool bOnlyContract)
{
	char strText[100] = "";
	if (ContractKey.CommodityType == ctFutures || ContractKey.CommodityType == ctGoods || ContractKey.CommodityType == ctDefer)//�ڻ�,�ֻ����ֻ�����
	{
		if (!bOnlyContract)
			sprintf_s(strText, "%s %s %s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo);
		else
			sprintf_s(strText, "%s", ContractKey.ContractNo);
	}
	else if (ContractKey.CommodityType == ctOption)//��Ȩ
	{
		if (!bOnlyContract)
			sprintf_s(strText, "%s %s %s %c%s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice);
		else
			sprintf_s(strText, "%s %c%s", ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice);
	}
	else if (ContractKey.CommodityType == ctSpreadMonth)//�ڻ��� ��������
	{
		if (!bOnlyContract)
			sprintf_s(strText, "%s %s %s/%s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.ContractNo2);
		else
			sprintf_s(strText, "%s/%s", ContractKey.ContractNo, ContractKey.ContractNo2);
	}
	else if (ContractKey.CommodityType == ctSpreadCommodity)//����Ʒ����
	{
		if (!bOnlyContract)
			sprintf_s(strText, "%s %s %s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo);
		else
			sprintf_s(strText, "%s", ContractKey.ContractNo);
	}
	else if (ContractKey.CommodityType == ctBUL || ContractKey.CommodityType == ctBER || ContractKey.CommodityType == ctBLT || ContractKey.CommodityType == ctBRT || ContractKey.CommodityType == ctSTD || ContractKey.CommodityType == ctSTG)//��Ȩ�� ������Ȩ��ֱ�۲������Ȩ��ֱ�۲��ʽ��ϣ�����ʽ��ϣ�ˮƽ����������ˮƽ��������	
	{
		if (!bOnlyContract)
			sprintf_s(strText, "%s %s %s %c%s/%s %c%s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice, ContractKey.ContractNo2, ContractKey.OptionType2, ContractKey.StrikePrice2);
		else
			sprintf_s(strText, "%s %c%s/%s %c%s", ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice, ContractKey.ContractNo2, ContractKey.OptionType2, ContractKey.StrikePrice2);
	}
	strCode = strText;
	return true;
}
bool CConvert::GetTradeCommodityInf(const TCommodityKey &sCommodityKey, SOFCommodityInf & sCommodityInfo)
{
	if (!(NULL != g_pTradeData && NULL != g_pTradeData->GetCommodity(&sCommodityKey)))
	{
		sCommodityInfo.bCanNegative = false;
		sCommodityInfo.dMiniChangePrice = 1;
		sCommodityInfo.iPrecision = 0;
		sCommodityInfo.bIfFractionPrice = false;
		return false;
	}
	const SCommodity * pCommodityInfo = g_pTradeData->GetCommodity(&sCommodityKey);//�ɲ������ݸ���
	if (0 == pCommodityInfo->PriceDeno)
	{
		sCommodityInfo.bCanNegative = false;
		sCommodityInfo.dMiniChangePrice = 1;
		sCommodityInfo.iPrecision = 0;
		return true;
	}
	strncpy_s(sCommodityInfo.Sign, sCommodityKey.Sign, _TRUNCATE);
	char CommdityType = sCommodityKey.CommodityType;
	if (ctGoods == CommdityType || ctFutures == CommdityType || ctOption == CommdityType)
		sCommodityInfo.bCanNegative = false;
	else if (ctSpreadMonth == CommdityType || ctSpreadCommodity == CommdityType ||
		ctBUL == CommdityType || ctBER == CommdityType || ctBLT == CommdityType || ctBRT == CommdityType || ctSTD == CommdityType || ctSTG == CommdityType)
		sCommodityInfo.bCanNegative = true;

	//��ƽ��Ϣ
	sCommodityInfo.cCoverMode = pCommodityInfo->CoverMode;
	switch (pCommodityInfo->PriceDeno)
	{
	case 1:	//��Ӧ��С�䶯����С��
	{
		sCommodityInfo.bIfFractionPrice = false;
		sCommodityInfo.dMiniChangePrice = pCommodityInfo->PriceTick;//��С�䶯��
		sCommodityInfo.iPrecision = pCommodityInfo->PricePrec;
		return true;
	}
	default: //��Ӧ��С�䶯���Ƿ��� ����10/320 ����û�о��� ����û���� ���ڶ��ٲ�֪��
	{
		sCommodityInfo.bIfFractionPrice = true;
		sCommodityInfo.dMiniChangePrice = pCommodityInfo->PriceNume;//���ӱ䶯��λ ��������
		sCommodityInfo.iPrecision = pCommodityInfo->PriceDeno;	//��ĸ
		break;
	}
	}
	return true;
}

bool CConvert::TContractSingleKeyToTContractNo(const TContractSingleKey & SingContractKey, string & strCode)//
{
	char str[80] = "";
	if (SingContractKey.CommodityType == ctFutures || SingContractKey.CommodityType == ctGoods || SingContractKey.CommodityType == ctDefer)//�ڻ�,�ֻ�
		sprintf_s(str, "%s %s %s", SingContractKey.ExchangeNo, SingContractKey.CommodityNo, SingContractKey.ContractNo);
	else if (SingContractKey.CommodityType == ctOption)//��Ȩ
		sprintf_s(str, "%s %s %s %c%s", SingContractKey.ExchangeNo, SingContractKey.CommodityNo, SingContractKey.ContractNo, SingContractKey.OptionType, SingContractKey.StrikePrice);
	else
		return false;
	strCode = str;
	return true;
}


//�ж��Ƿ�����Ч�Ľ��׺�Լ
bool CConvert::IsValidTradeCommodity(const TCommodityKey & CommodityKey)
{
	const SCommodity *pCommodity = g_pTradeData->GetCommodity(&CommodityKey);
	if (pCommodity)	return true;
	return false;
}

//�ж��Ƿ�����Ч������Ʒ��
bool CConvert::IsValidQuoteCommodity(const char * CommodityId)
{
	SCommodity *SCommodity = NULL;
	if (g_pStarApi&&g_pStarApi->GetCommodityData("", CommodityId, &SCommodity, 1, false))
		return true;
	return false;
}

//�Ƿ���Ч�����Լ
bool CConvert::IsValidQuotContract(const  char * ContractId)
{
	SContract *SContract = NULL;
	if (g_pStarApi&&g_pStarApi->GetContractData("", ContractId, &SContract, 1, false))
		return true;
	return false;
}

//�ж��Ƿ�����Ϻ�Լ
bool CConvert::IsCombContract(const char CommodityType)
{
	if (ctSpreadMonth == CommodityType || ctSpreadCommodity == CommodityType || ctSpreadLocal == CommodityType ||
		ctBUL == CommodityType || ctBER == CommodityType || ctBLT == CommodityType || ctBRT == CommodityType ||
		ctSTD == CommodityType || ctSTG == CommodityType)
		return true;
	return false;
}

//��Ϻ�Լ�Ƿ��Ե�һ��Ϊ׼��1�ǣ�0�Եڶ���Ϊ׼
bool CConvert::CombContractIsFirst(const TCommodityKey & CommodityKey)
{
	const SCommodity *pCommodity = g_pTradeData->GetCommodity(&CommodityKey);
	if (!pCommodity)	return true;
	if (dfFirst == pCommodity->SpreadDirect)
		return true;
	else if (dfSecond == pCommodity->SpreadDirect)
		return false;
	else
		return true;
}

bool CConvert::SCommodityNoToTCommodityKey(SCommodityNoType contractno, TCommodityKey &CommodityKey)
{
	memset(&CommodityKey, 0, sizeof(TCommodityKey));

	char* pos = strchr(contractno, '|');
	if (NULL == pos)
		return false;
	strncpy_s(CommodityKey.ExchangeNo, contractno, pos - contractno);
	CommodityKey.CommodityType = QuoteTypeToTradeType(pos[1]);

	//��Ʒ��
	char* pos2 = strchr(&pos[3], '|');
	if (NULL == pos2)
		strncpy_s(CommodityKey.CommodityNo, &pos[3], sizeof(SCommodityNoType) - (&pos[3] - contractno));
	else
		strncpy_s(CommodityKey.CommodityNo, &pos[3], pos2 - &pos[3]);

	return true;
}
void CConvert::CombTContractKeyToTContractSingleKey(const TContractKey &ContractKey, TContractSingleKey &SigCont1, TContractSingleKey &SigCont2)
{
	const SCommodity *pCommodity = g_pTradeData->GetCommodity(&ContractKey);
	if (!pCommodity)	return;
	bool bFirstDirect = CombContractIsFirst(ContractKey);
	if (bFirstDirect)
	{
		//1		
		if (pCommodity->TargetCommodity[0])
			SCommodityNoToTCommodityKey(pCommodity->TargetCommodity[0]->CommodityNo, SigCont1);
		strcpy_s(SigCont1.Sign, ContractKey.Sign);
		strcpy_s(SigCont1.ContractNo, ContractKey.ContractNo);
		strcpy_s(SigCont1.StrikePrice, ContractKey.StrikePrice);
		SigCont1.OptionType = ContractKey.OptionType;
		//2	
		if (pCommodity->TargetCommodity[1])
			SCommodityNoToTCommodityKey(pCommodity->TargetCommodity[1]->CommodityNo, SigCont2);
		strcpy_s(SigCont2.Sign, ContractKey.Sign);
		strcpy_s(SigCont2.ContractNo, ContractKey.ContractNo2);
		strcpy_s(SigCont2.StrikePrice, ContractKey.StrikePrice2);
		SigCont2.OptionType = ContractKey.OptionType2;
	}
	else//�Եڶ���Ϊ׼
	{
		//1		
		if (pCommodity->TargetCommodity[1])
			SCommodityNoToTCommodityKey(pCommodity->TargetCommodity[1]->CommodityNo, SigCont1);
		strcpy_s(SigCont1.Sign, ContractKey.Sign);
		strcpy_s(SigCont1.ContractNo, ContractKey.ContractNo2);
		strcpy_s(SigCont1.StrikePrice, ContractKey.StrikePrice2);
		SigCont1.OptionType = ContractKey.OptionType2;
		//2		
		if (pCommodity->TargetCommodity[0])
			SCommodityNoToTCommodityKey(pCommodity->TargetCommodity[0]->CommodityNo, SigCont2);
		strcpy_s(SigCont2.Sign, ContractKey.Sign);
		strcpy_s(SigCont2.ContractNo, ContractKey.ContractNo);
		strcpy_s(SigCont2.StrikePrice, ContractKey.StrikePrice);
		SigCont2.OptionType = ContractKey.OptionType;
	}
}

string CConvert::GetStrPriceByPrice(double price, bool bFraction, int nPrecision)
{
	string str;
	if (price == 0)
		return str;
	SFractionPrice tempPrice;
	char sText[64];
	if (bFraction)
	{
		DecimalsToFractions(price, nPrecision, tempPrice);//	
		if (tempPrice.iInteger > 0)
			sprintf_s(sText, "%d+%d/%d", tempPrice.iInteger, tempPrice.iMolecules, tempPrice.idenominator);
		else if (tempPrice.iInteger < 0)
			sprintf_s(sText, "%d-%d/%d", tempPrice.iInteger, abs(tempPrice.iMolecules), tempPrice.idenominator);
		else
			sprintf_s(sText, "%d/%d", tempPrice.iMolecules, tempPrice.idenominator);
	}
	else
		sprintf_s(sText, "%0.*lf", nPrecision, price);
	str = sText;
	return str;
}

bool CConvert::DecimalsToFractions(double inPrice, int denominator, SFractionPrice & out)
{
	char cMolecular[11] = { 0 };
	int   iInteger = 0;
	double fDecimal = 0;							//�������������ֺ�С������
	iInteger = (int)inPrice;
	fDecimal = inPrice - (float)iInteger;
	double fPrice = fDecimal*denominator;
	sprintf_s(cMolecular, "%0.*lf", 0, fPrice);	//����������Ӳ��� ,��ĸ���־���Precision

	out.iInteger = iInteger;						//��������
	out.iMolecules = (int)atof(cMolecular);		//���� ��ĸ����Precision MiniChangePrice��ÿ��Ҫ�䶯�ĵ�λ
	out.idenominator = denominator;
	return true;
}

string CConvert::FormatPriceToStr(double inPrice, SOFCommodityInf &mInfo)
{
	char sPrice[50];
	if (mInfo.bIfFractionPrice)
	{
		SFractionPrice sfPrice;
		DecimalsToFractions(inPrice, mInfo.iPrecision, sfPrice);
		sprintf_s(sPrice, "%d+%d/%d", sfPrice.iInteger, sfPrice.iMolecules, sfPrice.idenominator);
	}
	else
	{
		char format[50];
		sprintf_s(format, "%%0.%df", mInfo.iPrecision);//��һ��
		sprintf_s(sPrice, format, inPrice);
	}
	return sPrice;
}
//�Ƿ�����
bool CConvert::IsDiv(const double Value, const double SubValue, const double ErrorDiff)
{
	double n = 0;
	double m = modf(abs(Value / SubValue), &n);
	return m < ErrorDiff || 1 - m < ErrorDiff; //���ʹֵ�������ʹֵ��С�����������
}

void CConvert::GetSwapPositionVol(const char * UserNo, TContractSingleKey &SigCont1, TContractSingleKey&SigCont2,
	PosCount&posCount)
{
	if (UserNo == NULL)
		return;
	uint LongCount1 = 0, ShortCount1 = 0, DayLongCount1 = 0, DayShortCount1 = 0;
	uint LongCount2 = 0, ShortCount2 = 0, DayLongCount2 = 0, DayShortCount2 = 0;
	g_pTradeData->GetPositionVol(UserNo, &SigCont1, LongCount1, ShortCount1, DayLongCount1, DayShortCount1);
	g_pTradeData->GetPositionVol(UserNo, &SigCont2, LongCount2, ShortCount2, DayLongCount2, DayShortCount2);

	//���뿪�� ��һ�����뿪��(�ֵ�һ����) 
	posCount.BuyOpenCount = LongCount1;
	//����ƽ�� ��һ������ƽ�� �ڶ�����������(�ֵڶ�����)
	posCount.BuyCoverCount = ShortCount2;
	//�������� ��һ����������(�ֵ�һ����) 
	posCount.SellOpenCount = ShortCount1;
	//����ƽ�� ��һ������ƽ�� �ڶ������뿪��(�ֵڶ�����)
	posCount.SellCoverCount = LongCount2;
}

void CConvert::GetPositionVol(const char * UserNo, const TContractKey * ContractKey
	, uint &LongCount, uint &ShortCount, uint &DayLongCount, uint &DayShortCount)
{
	if (!g_pTradeData)
		return;
	if (UserNo == NULL || ContractKey == NULL)
	{
		g_pTradeData->GetPositionVol(NULL, NULL, LongCount, ShortCount, DayLongCount, DayShortCount);
		return;
	}

	if (!CConvert::IsCombContract(ContractKey->CommodityType))
		g_pTradeData->GetPositionVol(UserNo, ContractKey, LongCount, ShortCount, DayLongCount, DayShortCount);
	else
	{
		TContractSingleKey SigCont1, SigCont2;
		CConvert::CombTContractKeyToTContractSingleKey(*ContractKey, SigCont1, SigCont2);
		uint LongCount1 = 0, ShortCount1 = 0, DayLongCount1 = 0, DayShortCount1 = 0;
		uint LongCount2 = 0, ShortCount2 = 0, DayLongCount2 = 0, DayShortCount2 = 0;
		g_pTradeData->GetPositionVol(UserNo, &SigCont1, LongCount1, ShortCount1, DayLongCount1, DayShortCount1);
		g_pTradeData->GetPositionVol(UserNo, &SigCont2, LongCount2, ShortCount2, DayLongCount2, DayShortCount2);
		//��ͷ
		LongCount = LongCount1 > ShortCount2 ? ShortCount2 : LongCount1;
		DayLongCount = DayLongCount1 > DayShortCount2 ? DayShortCount2 : DayLongCount1;

		//��ͷ
		ShortCount = ShortCount1 > LongCount2 ? LongCount2 : ShortCount1;
		DayShortCount = DayShortCount1 > DayLongCount2 ? DayLongCount2 : DayShortCount1;
	}
}

bool  CConvert::OrderOffsetIsOpen(char direct, const char * UserNo, const TContractKey * ContractKey, char OrderType)//�ж��ǿ��ֻ���ƽ��
{
	if (direct == dBuy)	direct = dSell;
	else				direct = dBuy;
	int ccnum = 0;		//��ֲ�
	int cczuonum = 0;		//��ֲ�

	//��������
	if (OrderType == otSwap&&IsCombContract(ContractKey->CommodityType))
	{
		//��ȡ�����ֲ�
		TContractSingleKey SigCont1, SigCont2;
		CombTContractKeyToTContractSingleKey(*ContractKey, SigCont1, SigCont2);
		PosCount posCount;
		GetSwapPositionVol(UserNo, SigCont1, SigCont2, posCount);
		if (dBuy == direct)
		{
			if (posCount.SellOpenCount != 0)//˵������ƽ��
				ccnum = posCount.SellOpenCount;
		}
		else
		{
			if (posCount.BuyOpenCount != 0)//˵������ƽ��
				ccnum = posCount.SellOpenCount;
		}
	}
	else
	{
		uint LongCount = 0, ShortCount = 0, DayLongCount = 0, DayShortCount = 0;
		GetPositionVol(UserNo, ContractKey, LongCount, ShortCount, DayLongCount, DayShortCount);
		if (direct == dBuy)
		{
			ccnum = DayLongCount;
			cczuonum = LongCount;
		}
		else if (direct == dSell)
		{
			ccnum = ShortCount;
			cczuonum = DayShortCount;
		}
	}
	if (ccnum == 0 && cczuonum == 0)//û�ֲ֣��ǿ϶��ǿ�����
		return true;
	return false;
}
//���ݳֲֻ�ȡ��ƽ
TOffset CConvert::GetOffset(const char* UserNo, const TContractSingleKey * ContractKey, TDirect direct)
{
	uint LongCount = 0, ShortCount = 0, DayLongCount = 0, DayShortCount = 0;
	g_pTradeData->GetPositionVol(UserNo, ContractKey, LongCount, ShortCount, DayLongCount, DayShortCount);
	SOFCommodityInf sCommodityInfo;
	CConvert::GetTradeCommodityInf(*ContractKey, sCommodityInfo);
	if (sCommodityInfo.cCoverMode == cmCoverToday)
	{
		if ((dBuy == direct&&DayShortCount > 0) || (dSell == direct&&DayLongCount > 0))
			return oCoverT;
		else if ((dBuy == direct&&ShortCount > 0) || (dSell == direct&&LongCount > 0))
			return oCover;
		else
			return oOpen;	
	}
	else if (sCommodityInfo.cCoverMode == cmCover)
	{
		if ((dBuy == direct && ShortCount > 0) || (dSell == direct && LongCount > 0))
			return oCover;
		else
			return oOpen;
	}
	else
		return oNone;
}
TOffset CConvert::GetOffset(const char* UserNo, const TContractKey * ContractKey, TDirect direct)
{
	uint LongCount = 0, ShortCount = 0, DayLongCount = 0, DayShortCount = 0;
	GetPositionVol(UserNo, ContractKey, LongCount, ShortCount, DayLongCount, DayShortCount);
	SOFCommodityInf sCommodityInfo;
	CConvert::GetTradeCommodityInf(*ContractKey, sCommodityInfo);
	if (sCommodityInfo.cCoverMode == cmCoverToday)
	{
		if ((dBuy == direct&&DayShortCount > 0) || (dSell == direct&&DayLongCount > 0))
			return oCoverT;
		else if ((dBuy == direct&&ShortCount > 0) || (dSell == direct&&LongCount > 0))
			return oCover;
		else
			return oOpen;
	}
	else if (sCommodityInfo.cCoverMode == cmCover)
	{
		if ((dBuy == direct && ShortCount > 0) || (dSell == direct && LongCount > 0))
			return oCover;
		else
			return oOpen;
	}
	else
		return oNone;
}
//�������ʽƥ��
bool CConvert::MatchPattern(const char *pattern, const char *target)//����ƥ��
{
	std::tr1::cmatch res;
	std::tr1::regex rx(pattern);
	std::tr1::regex_search(target, res, rx);
	return !res.empty();
}

bool CConvert::DoubleSameValue(const double d1, const double d2, double Epsilon)
{
	if (Epsilon == 0)
		Epsilon = max(min(abs(d1), abs(d2)) * (1e-15) * 1000, (1e-15) * 1000);
	if (d1 > d2)
		return (d1 - d2) <= Epsilon;
	else
		return (d2 - d1) <= Epsilon;
};

bool CConvert::SplitNotice(const string &strContent, char cSplit, vector<NoticeContent> &vecstr)
{
	vecstr.clear();
	string sContent = strContent;
	string::size_type isplit = sContent.find(cSplit);
	NoticeContent tmp;
	if (string::npos == isplit)
	{
		string::size_type iIdent = strContent.find('=');
		tmp.strKey = strContent.substr(0, iIdent);
		tmp.strContent = strContent.substr(++iIdent, string::npos);
		vecstr.push_back(tmp);
		return true;
	}
	while (string::npos != isplit)
	{
		string strtmp = sContent.substr(0, isplit++);
		string::size_type iIdent = strtmp.find('=');
		if (string::npos != iIdent)
		{
			tmp.strKey = strtmp.substr(0, iIdent);
			tmp.strContent = strtmp.substr(++iIdent, string::npos);
			vecstr.push_back(tmp);
		}

		sContent = sContent.substr(isplit, string::npos);
		isplit = sContent.find(cSplit);
		if (isplit == string::npos)
		{
			iIdent = sContent.find('=');
			if (string::npos != iIdent)
			{
				tmp.strKey = sContent.substr(0, iIdent);
				tmp.strContent = sContent.substr(++iIdent, string::npos);
				vecstr.push_back(tmp);
			}
		}
	}
	return true;
}

bool CConvert::SplitNotice(const std::string & strContent, char cSplit, char cSplit2, std::vector<NoticeContent>& vecstr)
{
	vecstr.clear();
	std::string sContent = strContent;
	std::string::size_type isplit = sContent.find(cSplit);
	NoticeContent tmp;
	if (std::string::npos == isplit)
	{
		std::string::size_type iIdent = strContent.find(cSplit2);
		tmp.strKey = strContent.substr(0, iIdent);
		tmp.strContent = strContent.substr(++iIdent, std::string::npos);
		vecstr.push_back(tmp);
		return true;
	}
	while (std::string::npos != isplit)
	{
		std::string strtmp = sContent.substr(0, isplit++);
		std::string::size_type iIdent = strtmp.find(cSplit2);
		if (std::string::npos != iIdent)
		{
			tmp.strKey = strtmp.substr(0, iIdent);
			tmp.strContent = strtmp.substr(++iIdent, std::string::npos);
			vecstr.push_back(tmp);
		}

		sContent = sContent.substr(isplit, std::string::npos);
		isplit = sContent.find(cSplit);
		if (isplit == std::string::npos)
		{
			iIdent = sContent.find(cSplit2);
			if (std::string::npos != iIdent)
			{
				tmp.strKey = sContent.substr(0, iIdent);
				tmp.strContent = sContent.substr(++iIdent, std::string::npos);
				vecstr.push_back(tmp);
			}
		}
	}
	return true;
}

bool CConvert::SplitNoticePos(const string &strContent, char cSplit, vector<NoticeContent> &vecstr,int nContNum)
{
	string sContent = strContent;
	string::size_type npos = sContent.find(';');
	if (string::npos != npos)
	{
		sContent = sContent.substr(++npos, string::npos);
		npos = sContent.find(';');
		NoticeContent tmp;
		for (int i = 0; i < nContNum-2; i++)
		{
			if (string::npos != npos)
			{
				string strtmp = sContent.substr(0, npos++);
				string::size_type iIdent = strtmp.find('=');
				if (string::npos != iIdent)
				{
					tmp.strKey = strtmp.substr(0, iIdent);
					tmp.strContent = strtmp.substr(++iIdent, string::npos);
					vecstr.push_back(tmp);
				}
				else
					return false;
				sContent = sContent.substr(npos, string::npos);
				npos = sContent.find(';');
			}
			else
				return false;
		}
		string::size_type iIdent = sContent.find('=');
		if (string::npos != iIdent)
		{
			tmp.strKey = sContent.substr(0, iIdent);
			tmp.strContent = sContent.substr(++iIdent, string::npos);
			vecstr.push_back(tmp);
		}
		else
			return false;
	}
	else
		return false;

	return true;

}

vector<string> CConvert::SplitString(const string & str, char cSplit)
{
	vector<string> vecstr;
	string tempstr(str);
	string::iterator tempit = tempstr.begin();
	while (tempit != tempstr.end())
	{
		tempit = find<string::iterator, char>(tempstr.begin(), tempstr.end(), cSplit);
		if (tempit != tempstr.end())
		{
			iterator_traits<string::iterator>::value_type dis = distance(tempstr.begin(), tempit);
			vecstr.push_back(tempstr.substr(0, dis));
			if (++tempit != tempstr.end())
			{
				tempstr.erase(tempstr.begin(), tempit);
				tempit = tempstr.begin();
			}
			else
				break;
		}
		else//û�ҵ�' '��������  ����abc;������ȻҪ����
		{
			iterator_traits<string::iterator>::value_type dis = distance(tempit, tempstr.end());
			vecstr.push_back(tempstr);
			break;
		}
	}
	return vecstr;
}
TCommodityType CConvert::QuoteTypeToTradeType(char cType)
{
	switch (cType)
	{
	case S_COMMODITYTYPE_NONE:				return ctNone;
	case S_COMMODITYTYPE_SPOT:				return ctGoods;
	case S_COMMODITYTYPE_DEFER:				return ctDefer;
	case S_COMMODITYTYPE_FUTURES:			return ctFutures;
	case S_COMMODITYTYPE_OPTION:			return ctOption;
	case S_COMMODITYTYPE_MONTH:				return ctSpreadMonth;
	case S_COMMODITYTYPE_COMMODITY:			return ctSpreadCommodity;
	case S_COMMODITYTYPE_BUL:				return ctBUL;
	case S_COMMODITYTYPE_BER:				return ctBER;
	case S_COMMODITYTYPE_STD:				return ctSTD;
	case S_COMMODITYTYPE_STG:				return ctSTG;
	case S_COMMODITYTYPE_PRT:				return ctPRT;
	case S_COMMODITYTYPE_BLT:				return ctBLT;
	case S_COMMODITYTYPE_BRT:				return ctBRT;
	case S_COMMODITYTYPE_INDIRECT:			return ctInDirect;
	case S_COMMODITYTYPE_DIRECT:			return ctDirect;
	case S_COMMODITYTYPE_CROSS:				return ctCross;
	case S_COMMODITYTYPE_INDEX:				return ctIndexNumber;
	case S_COMMODITYTYPE_STOCK:				return ctStocks;
	default:
		return cType;
	}
}
//����Ʒ������ת����
SCommodityTypeType CConvert::TradeTypeToQuoteType(char cType)
{
	switch (cType)
	{
	case ctNone:				return S_COMMODITYTYPE_NONE;
	case ctGoods:				return S_COMMODITYTYPE_SPOT;
	case ctFutures:				return S_COMMODITYTYPE_FUTURES;
	case ctOption:				return S_COMMODITYTYPE_OPTION;
	case ctSpreadMonth:			return S_COMMODITYTYPE_MONTH;
	case ctSpreadCommodity:		return S_COMMODITYTYPE_COMMODITY;
	case ctBUL:					return S_COMMODITYTYPE_BUL;
	case ctBER:					return S_COMMODITYTYPE_BER;
	case ctSTD:					return S_COMMODITYTYPE_STD;
	case ctSTG:					return S_COMMODITYTYPE_STG;
	case ctPRT:					return S_COMMODITYTYPE_PRT;
	case ctBLT:					return S_COMMODITYTYPE_BLT;
	case ctBRT:					return S_COMMODITYTYPE_BRT;
	case ctDirect:				return S_COMMODITYTYPE_DIRECT;
	case ctInDirect:			return S_COMMODITYTYPE_INDIRECT;
	case ctCross:				return S_COMMODITYTYPE_CROSS;
	case ctIndexNumber:			return S_COMMODITYTYPE_INDEX;
	case ctStocks:				return S_COMMODITYTYPE_STOCK;
	default:
		return cType;
	}
}
void CConvert::SContractNoToTCommodityKey(const char *szSign, const std::string &id, TCommodityKey &CommodityKey)
{
	memset(&CommodityKey, 0, sizeof(TCommodityKey));
	strcpy_s(CommodityKey.Sign, szSign);
	std::vector<std::string> vecStr;
	vecStr = SplitString(id.c_str(), '|');
	if (vecStr.size() < 3)
		return;
	strncpy_s(CommodityKey.ExchangeNo, vecStr[0].c_str(), sizeof(CommodityKey.ExchangeNo) - 1);
	CommodityKey.CommodityType = QuoteTypeToTradeType(vecStr[1].c_str()[0]);
	strncpy_s(CommodityKey.CommodityNo, vecStr[2].c_str(), sizeof(CommodityKey.CommodityNo) - 1);
}
//����IDת���׹ؼ���
void CConvert::SContractNoToTContractKey(const char *szSign, const string &id, TContractKey &ContractKey)
{
	std::vector<std::string> vecStr;
	memset(&ContractKey, 0, sizeof(TContractKey));

	strcpy_s(ContractKey.Sign, szSign);
	ContractKey.ExchangeNo;
	ContractKey.CommodityType = ctNone;
	ContractKey.CommodityNo;
	ContractKey.ContractNo;
	ContractKey.StrikePrice;
	ContractKey.OptionType = otNone;
	ContractKey.ContractNo2;
	ContractKey.StrikePrice2;
	ContractKey.OptionType2 = otNone;

	vecStr = SplitString(id.c_str(), '|');
	if (vecStr.size() < 3)
		return;

	strncpy_s(ContractKey.ExchangeNo, vecStr[0].c_str(), sizeof(ContractKey.ExchangeNo) - 1);
	ContractKey.CommodityType = QuoteTypeToTradeType(vecStr[1].c_str()[0]);
	strncpy_s(ContractKey.CommodityNo, vecStr[2].c_str(), sizeof(ContractKey.CommodityNo) - 1);
	//����������ֶ�
	switch (ContractKey.CommodityType)
	{
		//�ڻ� <Exg>|<Type>|<Root>|<YEAR><MONTH>[DAY]
	case ctFutures:
		//ָ��
	case ctIndexNumber:
		if (vecStr.size() > 3)
			strncpy_s(ContractKey.ContractNo, vecStr[3].c_str(), sizeof(ContractKey.ContractNo) - 1);
		break;
		//��Ȩ	<Exg>|<Type>|<Root>|<YEAR><MONTH>[DAY]<OFlag><STRIKEPRICE>
	case ctOption:
		if (vecStr.size() > 3)
			ParseOptionContractStr(vecStr[3], ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice);
		break;
		//�������� <Exg>|<Type>|<Root>|<YEAR><MONTH>[DAY]|<YEAR><MONTH>[DAY]
	case ctSpreadMonth:
		if (vecStr.size() > 4)
		{
			strncpy_s(ContractKey.ContractNo, vecStr[3].c_str(), sizeof(ContractKey.ContractNo) - 1);
			strncpy_s(ContractKey.ContractNo2, vecStr[4].c_str(), sizeof(ContractKey.ContractNo2) - 1);
		}
		break;
		//��Ʒ������ <Exg>|<Type>|<Root&Root>|<YEAR><MONTH>[DAY]
	case ctSpreadCommodity:
		if (vecStr.size() > 3)
		{
			strncpy_s(ContractKey.ContractNo, vecStr[3].c_str(), sizeof(ContractKey.ContractNo) - 1);
			strncpy_s(ContractKey.ContractNo2, vecStr[3].c_str(), sizeof(ContractKey.ContractNo2) - 1);
		}
		break;
		//���Ǵ�ֱ����
	case ctBUL:
		//������ֱ����
	case ctBER:
		//��ʽ����
	case ctSTD:
		//����ʽ����
	case ctSTG:
		//��Ȩ����	<Exg>|<Type>|<Root[&Root]>|<YEAR><MONTH>[DAY]<OFlag><STRIKEPRICE>|<YEAR><MONTH>[DAY]<OFlag><STRIKEPRICE>
		if (vecStr.size() > 4)
		{
			ParseOptionContractStr(vecStr[3], ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice);
			ParseOptionContractStr(vecStr[4], ContractKey.ContractNo2, ContractKey.OptionType2, ContractKey.StrikePrice2);
		}
		break;
		//�������
	case ctPRT:
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
		break;
	default:
		break;
	}
}
//���׺�ԼתΪ����ID
void CConvert::TContractKeyToSContractNo(const TContractKey &ContractKey, char(&ContractId)[101])
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
		//����ʽ����
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
	case ctDefer:
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
//���׺�ԼתΪ����ID
void CConvert::TContractSingleKeyToSContractNo(const TContractSingleKey &ContractKey, char(&ContractId)[101])
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
		//�������
	case ctPRT:
		break;
		//�ֻ�
	case ctGoods:
	case ctDefer:
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
//��ȡ��С�䶯�۸�
double CConvert::GetMinChangePrice(const SOFCommodityInf & sCommodityInfo)
{
	double MiniChangePrice = sCommodityInfo.dMiniChangePrice;
	if (sCommodityInfo.bIfFractionPrice)//��������
		MiniChangePrice = (sCommodityInfo.dMiniChangePrice / sCommodityInfo.iPrecision);
	return MiniChangePrice;
}
//API �������޹� ����0�ɹ� ���ظ���ʧ��  ��ѯ����ʧ��
int CConvert::QrySingleContractCanOpen(const TSendOrder &SendOrder)
{
	//Ӧ�ø���Direct������ɿ�����
	int canqty = 0;
	if (g_pTradeData)
	{
		int iret = g_pTradeData->GetTradableQty(SendOrder, canqty);
		if (0 != iret)
			canqty = -1;
	}

	return canqty;
}
//��ȡ�ɿ� ����ϵͳ������Ϣ���ֲ���Ϣ 
int CConvert::GetCanOpenQty(char cCoverMode, const TSendOrder &SendOrder, bool bOpenMaxAutoCancel)
{
	int uSigCanOpen = 0;

	//���ֿ�ƽ��
	bool bHaveCover = (cCoverMode == cmNone || cCoverMode == cmUnfinish) ? false : true;

	//���Ȼ������
	if (CConvert::IsCombContract(SendOrder.CommodityType))//�������� 
	{
		TContractSingleKey SigCont1, SigCont2;
		CConvert::CombTContractKeyToTContractSingleKey(SendOrder, SigCont1, SigCont2);

		//������Ͽɿ�  ҲȥAPI��ѯ  Ȼ����� �������ȵ���С�ɿ����� +API���ص�  �����㷨��׼ȷ
		uSigCanOpen = QrySingleContractCanOpen(SendOrder);

		//���ֹҵ���
		const TOrderQty *p1 = g_pTradeData->GetParOrderQty(SendOrder.UserNo, &SigCont1);
		const TOrderQty *p2 = g_pTradeData->GetParOrderQty(SendOrder.UserNo, &SigCont2);

		if (bHaveCover&&bOpenMaxAutoCancel)//���ֿ�ƽ��ѡ�п����Զ�����
		{
			int sig = 0;
			if (dBuy == SendOrder.Direct)
				sig = min((p1 ? p1->BuyOpenQty : 0), (p2 ? p2->SellOpenQty : 0));
			else
				sig = min((p1 ? p1->SellOpenQty : 0), (p2 ? p2->BuyOpenQty : 0));
			uSigCanOpen = (uSigCanOpen < 0) ? sig : (uSigCanOpen + sig);
			return uSigCanOpen;
		}
		else
			return uSigCanOpen;
	}
	else
	{
		//API��ѯ�ɿ�����
		uSigCanOpen = QrySingleContractCanOpen(SendOrder);
		if (uSigCanOpen < 0)
			return -1;
		//�ҵ���
		const TOrderQty *pOrderQty = g_pTradeData->GetParOrderQty(SendOrder.UserNo, &SendOrder);

		//����
		if (pOrderQty)
		{
			if (bHaveCover&&bOpenMaxAutoCancel)//���ֿ�ƽ��ѡ�п����Զ�����
			{
				if (dBuy == SendOrder.Direct)
					return uSigCanOpen + pOrderQty->BuyOpenQty;
				else
					return uSigCanOpen + pOrderQty->SellOpenQty;
			}
			else
				return uSigCanOpen;
		}
	}
	return uSigCanOpen;
}

//�����Ȩ�ֶ�
bool CConvert::ParseOptionContractStr(const std::string& strValue, char(&ContractNo)[11], TOptionType &OptionType, char(&StrikePrice)[11])
{
	bool bRet = true;
	do{
		string::size_type posFlag = std::string::npos;
		if (std::string::npos != (posFlag = strValue.find(otCall)))
			OptionType = otCall;
		else if (std::string::npos != (posFlag = strValue.find(otPut)))
			OptionType = otPut;
		else
		{
			bRet = false;
			break;
		}
		strncpy_s(ContractNo, strValue.substr(0, posFlag).c_str(), sizeof(ContractNo) - 1);
		strncpy_s(StrikePrice, strValue.substr(posFlag + 1).c_str(), sizeof(StrikePrice) - 1);
	} while (false);
	return bRet;
}
bool CConvert::GetQuoteCommodityInf(const char*code, SOFCommodityInf & sCommodityInfo)
{
	if (!g_pStarApi)
		return false;

	TContractKey	TradeContract;	//���׺�Լ
	memset(&TradeContract, 0, sizeof(TradeContract));
	CConvert::TContractNoToTContractKey(code, TradeContract);

	SCommodity *tmpCommodity = NULL;
	SCommodityNoType SCommodityNo = { 0 };
	sprintf_s(SCommodityNo, "%s|%c|%s", TradeContract.ExchangeNo, CConvert::TradeTypeToQuoteType(TradeContract.CommodityType), TradeContract.CommodityNo);
	int num = g_pStarApi->GetCommodityData("", SCommodityNo, &tmpCommodity, 1, false);

	if (0 == num)
	{
		sCommodityInfo.dMiniChangePrice = 1.0;
		return false;
	}

	//�������Ǹ���
	char CommdityType = TradeContract.CommodityType;
	if (ctGoods == CommdityType || ctFutures == CommdityType || ctOption == CommdityType)
		sCommodityInfo.bCanNegative = false;
	else if (ctSpreadMonth == CommdityType || ctSpreadCommodity == CommdityType ||
		ctBUL == CommdityType || ctBER == CommdityType || ctBLT == CommdityType || ctBRT == CommdityType || ctSTD == CommdityType || ctSTG == CommdityType)
		sCommodityInfo.bCanNegative = true;

	//�ж��Ƿ������ۻ���С������
	if (tmpCommodity->PriceDeno > 1)//��������
	{
		sCommodityInfo.bIfFractionPrice = true;
		sCommodityInfo.dMiniChangePrice = tmpCommodity->PriceNume;			//���ӱ䶯��λ ��������
		sCommodityInfo.iPrecision = tmpCommodity->PriceDeno;					//��ĸ
	}
	else //С������
	{
		sCommodityInfo.bIfFractionPrice = false;
		sCommodityInfo.iPrecision = tmpCommodity->PricePrec;
		sCommodityInfo.dMiniChangePrice = tmpCommodity->PriceTick;			//��С�䶯��
	}
	sCommodityInfo.ProductDot = tmpCommodity->TradeDot;

	return true;
}
//����id�������ַ���
void CConvert::SContractNoToTContractNo(string &sQuoteCode, string &str)
{
	TContractKey	TradeContract;	//���׺�Լ
	memset(&TradeContract, 0, sizeof(TradeContract));
	SContractNoToTContractKey("", sQuoteCode, TradeContract);
	TContractKeyToTContractNo(TradeContract, str);
}

void CConvert::QContractNoTypeToStr(std::string & sQuoteCode, std::string & str, bool bOnlyContract)
{
	TContractKey	TradeContract;	//���׺�Լ
	memset(&TradeContract, 0, sizeof(TradeContract));
	SContractNoToTContractKey("", sQuoteCode, TradeContract);
	TContractKeyToTContractNo2(TradeContract, str, bOnlyContract);
}

//�Ƿ�����ƽ����ƽ��(�Ϻ�) ����-1������  ֻ�����ֿ���ƽ�ֵĲ���Ҫ���ô˺���
bool CConvert::IsDistinguishCoverType(const TCommodityKey*pCommoditykey)
{
	const SCommodity * pCommodity = g_pTradeData->GetCommodity(pCommoditykey);
	if (!pCommodity)		
		return false;
	if (cmCoverToday == pCommodity->CoverMode)	return true;
		return false;
}

void  CConvert::GetPosNum(const char *UserNo, const TContractKey * ContractKey, char Direct,
	uint &iPosT, uint &iPos)
{
	//�Ƿ����ֽ���
	bool bDisCover = IsDistinguishCoverType(ContractKey);
	uint LongCount = 0, ShortCount = 0, DayLongCount = 0, DayShortCount = 0;
	GetPositionVol(UserNo, ContractKey, LongCount, ShortCount, DayLongCount, DayShortCount);
	if (Direct == dBuy)
	{
		if (bDisCover)
		{
			iPosT = DayLongCount;
			iPos = LongCount > DayLongCount ? LongCount - DayLongCount : 0;
		}
		else//������
		{
			iPosT = 0;
			iPos = LongCount;
		}
	}
	else if (Direct == dSell)
	{
		if (bDisCover)
		{
			iPosT = DayShortCount;
			iPos = ShortCount > DayShortCount ? ShortCount - DayShortCount : 0;
		}
		else//�����ֿ�ƽ
		{
			iPosT = 0;
			iPos = ShortCount;
		}
	}
}
//�õ��ֲ֡���ƽ ���ݿͻ� ���������� ���� һ��һƽ
void  CConvert::GetPosCanCoverNum(const char *UserNo, const TContractKey * ContractKey, char Direct, bool bDisCover,
	uint &iPosT, uint &iPos, uint &CanCoverNum, uint&CanCoverTNum)
{
	//����ĳЩ�������������ƽ���� �жϺ�Լ������ĳֲ�
	char DifDirect = dNone;
	DifDirect = (Direct == dBuy) ? dSell : dBuy;
	GetPosNum(UserNo, ContractKey, DifDirect, iPosT, iPos);

	//�õ���ƽ ���Ⱥ���ϲ�ͬ
	if (!IsCombContract(ContractKey->CommodityType))
	{
		const TOrderQty *pOrderQty = g_pTradeData->GetParOrderQty(UserNo, ContractKey);
		uint  QueueCount = 0, QueueDayCount = 0;//�ҵ���
		if (pOrderQty)
		{
			if (Direct == dBuy)
			{
				if (bDisCover)
				{
					QueueCount = pOrderQty->BuyCoverQty;
					QueueDayCount = pOrderQty->BuyCoverDayQty;
				}
				else
					QueueCount = pOrderQty->BuyCoverDayQty + pOrderQty->BuyCoverQty;
			}
			else if (Direct == dSell)
			{
				if (bDisCover)
				{
					QueueCount = pOrderQty->SellCoverQty;
					QueueDayCount = pOrderQty->SellCoverDayQty;
				}
				else
					QueueCount = pOrderQty->SellCoverDayQty + pOrderQty->SellCoverQty;
			}
		}

		//�����ƽ��Ŀ
		if (bDisCover)//���ֿ�ƽ
		{
			CanCoverNum = (iPos > 0 && iPos > QueueCount) ? (iPos - QueueCount) : 0;	//���ƽ
			CanCoverTNum = (iPosT > 0 && iPosT > QueueDayCount) ? (iPosT - QueueDayCount) : 0;	//���ƽ
		}
		else//�����ֿ�ƽ
		{
			CanCoverNum = (iPos > 0 && iPos > QueueCount) ? (iPos - QueueCount) : 0;//��ƽ
			CanCoverTNum = 0;
		}
	}
	else//��Ͽ�ƽ �����ֽ���
	{
		TContractSingleKey SigCont1, SigCont2;
		CombTContractKeyToTContractSingleKey(*ContractKey, SigCont1, SigCont2);
		uint LongCount1 = 0, ShortCount1 = 0, DayLongCount1 = 0, DayShortCount1 = 0;
		uint LongCount2 = 0, ShortCount2 = 0, DayLongCount2 = 0, DayShortCount2 = 0;
		g_pTradeData->GetPositionVol(UserNo, &SigCont1, LongCount1, ShortCount1, DayLongCount1, DayShortCount1);
		g_pTradeData->GetPositionVol(UserNo, &SigCont2, LongCount2, ShortCount2, DayLongCount2, DayShortCount2);

		const TOrderQty *pOrderQty1 = g_pTradeData->GetParOrderQty(UserNo, &SigCont1);
		const TOrderQty *pOrderQty2 = g_pTradeData->GetParOrderQty(UserNo, &SigCont2);

		CanCoverTNum = 0;
		if (Direct == dBuy)//��һ�ȳ�����ȥ��һ����ƽ�ҵ�A �ó���ƽ �ڶ��ȳ����ȥ�ڶ�����ƽ�ҵ�B min(A,B)�õ���������ƽ
			CanCoverNum = min(ShortCount1 - (pOrderQty1 ? (pOrderQty1->BuyCoverQty + pOrderQty1->BuyCoverDayQty) : 0), LongCount2 - (pOrderQty2 ? (pOrderQty2->SellCoverQty + pOrderQty2->SellCoverDayQty) : 0));
		else//��һ�ȳ����ȥ��һ����ƽ�ҵ�A �ڶ��ȳ�����ȥ�ڶ�����ƽ�ҵ�B min(A,B)�õ����������ƽ
			CanCoverNum = min(LongCount1 - (pOrderQty1 ? (pOrderQty1->SellCoverQty + pOrderQty1->SellCoverDayQty) : 0), ShortCount2 - (pOrderQty2 ? (pOrderQty2->BuyCoverQty + pOrderQty2->BuyCoverDayQty) : 0));
	}
}

//��ȡ��ƽ ����ϵͳ������Ϣ���ֲ���Ϣ 
void CConvert::GetCanCoverQty(char cCoverMode, const TContractKey &ContractKey, const char *UserNo, char Direct,
	CanCoverQty& CanCoverQty, bool bLiquidateAutoCancel)
{
	//��δ��������µ��Զ���ƽ���ܣ�

	uint ccTnum = 0, cczuonum = 0;		//��ֲ֣���ֲ�
	uint CanCoverNum = 0, CanCoverTNum = 0;//���ƽ�����ƽ

	//���ֿ�ƽ��
	bool bHaveCover = (cCoverMode == cmNone || cCoverMode == cmUnfinish) ? false : true;

	if (bHaveCover)
	{
		//��ȡ�ֲֺͿ�ƽ����
		bool bDisCover = (cCoverMode == cmCoverToday) ? 1 : 0;
		CConvert::GetPosCanCoverNum(UserNo, &ContractKey, Direct, bDisCover, ccTnum, cczuonum, CanCoverNum, CanCoverTNum);

		//���Ȼ������
		if (CConvert::IsCombContract(ContractKey.CommodityType))//�������� 
		{
			//���ֽ���Ľ�����û����� �˼��費����ʱ������������
			if (bLiquidateAutoCancel)
				CanCoverQty.uCanCoverQty = cczuonum;
			else
				CanCoverQty.uCanCoverQty = CanCoverNum;
		}
		else
		{
			//���� m_SysGeneralOrderSetting.bAutoSelectPJPZ;
			//ƽ��ƽ������Ӧ ����й�ϵ���й� �ڵ��ô˺����ĵط��Լ����� ��Ϊ����Ϻ�����ʾ��һ�� �Ϻ���ʾ��ƽ�򡢿�ƽ��
			if (bDisCover)
			{
				if (bLiquidateAutoCancel)
				{
					CanCoverQty.uCanCoverQty = cczuonum;
					CanCoverQty.uCanCoverTQty = ccTnum;
				}
				else
				{
					CanCoverQty.uCanCoverQty = CanCoverNum;
					CanCoverQty.uCanCoverTQty = CanCoverTNum;
				}
			}
			else//�����ֽ���
			{
				if (bLiquidateAutoCancel)
					CanCoverQty.uCanCoverQty = cczuonum;
				else
					CanCoverQty.uCanCoverQty = CanCoverNum;
			}
		}
	}
	else//����
	{
		//��ȡ�ֲ����� ���Ȼ�����϶�һ��
		uint uCanCover = 0;
		CanCoverQty.uCanCoverQty = uCanCover;
	}
}

bool CConvert::IsCoverOrCoverTCommodity(const TCommodityKey*pCommoditykey)
{
	const SCommodity * pCommodity = g_pTradeData->GetCommodity(pCommoditykey);
	if (!pCommodity)		return false;
	if (cmCoverToday == pCommodity->CoverMode|| cmCover == pCommodity->CoverMode)	return true;
	return false;
}
//�����ַ���������id
bool CConvert::TContractNoToSContractNo(const char *code, char(&ContractId)[101])
{
	TContractKey	TradeContract;	//���׺�Լ
	memset(&TradeContract, 0, sizeof(TradeContract));
	TContractNoToTContractKey(code, TradeContract);
	TContractKeyToSContractNo(TradeContract, ContractId);	
	return  IsValidQuotContract(ContractId);
	
}
void CConvert::TrimString(string &str)
{
	string::size_type pos = str.find_first_not_of(' ');
	if (pos == string::npos)
	{
		return;
	}
	string::size_type pos2 = str.find_last_not_of(' ');
	if (pos2 != string::npos)
	{
		str = str.substr(pos, pos2 - pos + 1);
		return;
	}
	str = str.substr(pos);
	return;
}


void CConvert::GetPriceQty(const SQuoteSnapShot& QuoteSnap, double& dbuy, double& dsell, SQtyType& nbuy, SQtyType& nsell)
{
	
}
wstring CConvert::LoadResString(int strID)
{
	if (g_pLanguage)
		return g_pLanguage->LangText(strID);
	else
		return L"";
}
long CConvert::Round(double value, double dDif)
{
	if (value > 0)
		return (long)(value + dDif);
	else
		return (long)(value - dDif);
};
//�ж����������Ƿ����
bool CConvert::SameValue(const double A, const double B, double Epsilon)
{
	if (Epsilon == 0)
		Epsilon = max(min(abs(A), abs(B)) * (1e-15) * 1000, (1e-15) * 1000);
	if (A > B)
		return (A - B) <= Epsilon;
	else
		return (B - A) <= Epsilon;
}
int CConvert::CmpValue(const double A, const double B, double Epsilon)
{
	if (SameValue(A, B, Epsilon))
		return 0;
	else if (A > B)
		return 1;
	else
		return -1;
}
SPriceType CConvert::GetPriceByContract(const SContract* SContract, SFidMeanType nField)//����������
{
	if (!SContract || !g_pStarApi)
		return 0;
	if (nField == S_FID_BESTBIDPRICE || nField == S_FID_BESTASKPRICE)
	{
		SQuoteField qInfo;
		if (g_pStarApi->GetImpliedField(SContract, nField, &qInfo))
			return qInfo.FidAttr ? qInfo.Price : 0;
		else
			return GetPriceBySnapShot(SContract->SnapShot, nField);
	}
	else
		return 0;
}
SQtyType CConvert::GetQtyByContract(const SContract* SContract, SFidMeanType nField)//����������
{
	if (!SContract || !g_pStarApi)
		return 0;
	if (nField == S_FID_BESTBIDQTY || nField == S_FID_BESTASKQTY)
	{
		SQuoteField qInfo;
		if (g_pStarApi->GetImpliedField(SContract, nField, &qInfo))
			return qInfo.FidAttr ? qInfo.Qty : 0;
		else
			return GetQtyBySnapShot(SContract->SnapShot, nField);
	}
	else
		return 0;
}
SPriceType CConvert::GetPriceBySnapShot(const SQuoteSnapShot* QuoteSnap, SFidMeanType nField)
{
	if (QuoteSnap)
		return QuoteSnap->Data[nField].FidAttr ? QuoteSnap->Data[nField].Price : 0;
	else
		return 0;
}
SQtyType CConvert::GetQtyBySnapShot(const SQuoteSnapShot* QuoteSnap, SFidMeanType nField)
{
	if (QuoteSnap)
		return QuoteSnap->Data[nField].FidAttr ? QuoteSnap->Data[nField].Qty : 0;
	else
		return 0;
}
double CConvert::FormatPriceToDouble(double dPrice, int nPrecision, double dMinChange)
{
	return int(Round(Round(dPrice*pow(10, nPrecision)) / pow(10, nPrecision) / dMinChange,FLOAT_SAME_ERRORDIFF)*dMinChange*(pow(10, nPrecision))) / pow(10, nPrecision);
}

wstring CConvert::UTF8ToUnicode(const string& str)
{
	size_t  len = 0;
	len = str.length();
	int  unicodeLen = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);

	wchar_t *  pUnicode;
	pUnicode = new  wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));

	::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, (LPWSTR)pUnicode, unicodeLen);
	wstring   rt = (wchar_t*)pUnicode;
	delete[]  pUnicode;

	return  rt;
}

bool CConvert::IsValidNumber(const char * sValue, bool bOnlyInt, bool bCanNegative)
{
	string pattern = "^(\\d+)?(\\.)?(\\d+)?$";	//������С��   
	if (bOnlyInt)
		pattern = "^(\\d+)?$";					//ֻ����Ϊ����    
	if (bCanNegative)
		pattern.insert(1, "(-)?");				//����Ϊ��  "^[-+]?(\\d+)?(\\.)?(\\d+)?$"

	return MatchPattern(pattern.c_str(), sValue);
}

bool CConvert::MatchPattern(const wchar_t * pattern, const wchar_t * target)
{
	std::wcmatch res;
	std::wregex rx(pattern);
	std::regex_search(target, res, rx);
	return !res.empty();
}

bool CConvert::IsValidNumber(const wchar_t * sValue, bool bOnlyInt, bool bCanNegative)
{
	wstring pattern = L"^(\\d+)?(\\.)?(\\d+)?$";	//������С��   
	if (bOnlyInt)
		pattern = L"^(\\d+)?$";					//ֻ����Ϊ����    
	if (bCanNegative)
		pattern.insert(1, L"(-)?");				//����Ϊ��  "^[-+]?(\\d+)?(\\.)?(\\d+)?$"

	return MatchPattern(pattern.c_str(), sValue);
}