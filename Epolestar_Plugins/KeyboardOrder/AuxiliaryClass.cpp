#include "stdafx.h"
#include "AuxiliaryClass.h"
#include "DataContainer_Trade.h"
#include "PlugLoad.h"

//Ʒ������
typedef char               TapCommodityTypeType;
//��
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

extern CPlugLoad g_PlugLoad;

template<size_t _size>
inline size_t strncpy_s(char(&_dest)[_size], const char * _source) throw()
{
	return strncpy_s(_dest, _size, _source, _size - 1);
}

namespace CAuxiliary
{
	//��ʼ��TSendOrder
	void DefaultSendOrder(TSendOrder&SendOrder)
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
		SendOrder.Hedge2 = hNone;
	}
	bool IsHK(const char *szCode)
	{
		if (!szCode)	return false;
		TContractKey temp;
		memset(&temp, 0, sizeof(TContractKey));
		StrToTradeContract(szCode, temp);
		if ((0 == strcmp(temp.ExchangeNo, "HKEX") || 0 == strcmp(temp.ExchangeNo, "HKFE"))/*&& temp.CommodityType == ctFutures*/)
			return true;
		return false;
	}
	//��½���׺� ȡ������������Ϣ 0==m_iVersionType��ʾ���� 1==m_iVersionType��ʾ���� 2==m_iVersionType��ʾ������ 
	int  ReadVersionType()
	{
		CDataContainer tempData;
		g_pTradeData->GetAllExchange(tempData);
		uint count = tempData.size();
		bool bInner = false, bFore = false;
		CDataContainer tempCommoditys;
		for (uint i = 0; i < count; i++)
		{
			const TExchange *pExchange = (const TExchange *)tempData.at(i);
			//test
			tempCommoditys.clear();
			g_pTradeData->GetCommoditys(pExchange, tempCommoditys);
			if (tempCommoditys.size()>0)
			{
				const TCommodity *pCommodity = (const TCommodity *)tempCommoditys.at(0);
				if (cmUnfinish == pCommodity->CoverMode || cmNone == pCommodity->CoverMode)
					bFore = true;
				else
					bInner = true;
			}
		}
		int iVersionType = 0;
		if (bInner)	iVersionType = 0;
		if (bFore)	iVersionType = 1;
		if (bInner&&bFore)	iVersionType = 2;
		return iVersionType;
	}
	//�ַ���ת���׺�Լ
	bool StrToTradeContract(const char * szCode, TContractKey & ContractKey)
	{
		vector<string> vecstr;		//�������������
		string tempstr(szCode);
		string::iterator tempit = tempstr.begin();
		while (tempit != tempstr.end())
		{
			tempit = find<string::iterator, char>(tempstr.begin(), tempstr.end(), ' ');
			if (tempit != tempstr.end())
			{
				iterator_traits<string::iterator>::distance_type dis = distance(tempstr.begin(), tempit);
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
				iterator_traits<string::iterator>::distance_type dis = distance(tempit, tempstr.end());
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
			ContractKey.CommodityType = ctGoods;
			int iPs = vecstr[1].find('(');
			if (string::npos != iPs)	ContractKey.CommodityType = ctDefer;
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
							ContractKey.CommodityType = ctSTG;				//STG 8-���ʽ���
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
	bool TradeContractToStr(const TContractKey & ContractKey, string & strCode, bool bOnlyContract, bool bName, bool bOnlyName)//
	{
		char strText[100] = { 0 };
		const TContract*pContract = g_pTradeData->GetContract(&ContractKey);
		const TCommodity*pCommodity = NULL;
		if (pContract)	pCommodity = pContract->pCommodity;
		const TExchange*pExchange = g_pTradeData->GetExchange(&ContractKey);
		if (bOnlyName)
		{
			if (!(pContract &&pCommodity &&pExchange))	return false;
			if (pCommodity->CommodityType)
			{
				//���
				if (ctSpreadMonth == pCommodity->CommodityType || ctSpreadCommodity == pCommodity->CommodityType || ctBUL == pCommodity->CommodityType || ctBER == pCommodity->CommodityType || ctBLT == pCommodity->CommodityType || ctBRT == pCommodity->CommodityType || ctSTD == pCommodity->CommodityType || ctSTG == pCommodity->CommodityType)
					sprintf_s(strText, "%s\\%s\\%s %s", LoadRC::unicode_to_ansi(pExchange->ExchangeName).c_str(), LoadRC::unicode_to_ansi(pCommodity->CommodityName).c_str(), LoadRC::unicode_to_ansi(pContract->ContractName).c_str(), "�۲�");
				else
					sprintf_s(strText, "%s\\%s\\%s", LoadRC::unicode_to_ansi(pExchange->ExchangeName).c_str(), LoadRC::unicode_to_ansi(pCommodity->CommodityName).c_str(), LoadRC::unicode_to_ansi(pContract->ContractName).c_str());
			}
			strCode = strText;
			return true;
		}
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
		else if (ContractKey.CommodityType == ctBUL || ContractKey.CommodityType == ctBER || ContractKey.CommodityType == ctBLT || ContractKey.CommodityType == ctBRT || ContractKey.CommodityType == ctSTD || ContractKey.CommodityType == ctSTG)//��Ȩ�� ������Ȩ��ֱ�۲������Ȩ��ֱ�۲��ʽ��ϣ����ʽ��ϣ�ˮƽ����������ˮƽ��������	
		{
			if (!bOnlyContract)
				sprintf_s(strText, "%s %s %s %c%s/%s %c%s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice, ContractKey.ContractNo2, ContractKey.OptionType2, ContractKey.StrikePrice2);
			else
				sprintf_s(strText, "%s %c%s/%s %c%s", ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice, ContractKey.ContractNo2, ContractKey.OptionType2, ContractKey.StrikePrice2);		
		}
		strCode = strText;
		return true;
	}
	//���׺�Լת�ַ��� ֻҪƷ�ֺͺ�Լ ��Ҫ������
	bool TradeContractToStr(string & strCode, const TContractKey & ContractKey)
	{
		char strText[100] = { 0 };
		const TExchange*pExchange = g_pTradeData->GetExchange(&ContractKey);
		if (ContractKey.CommodityType == ctFutures || ContractKey.CommodityType == ctGoods || ContractKey.CommodityType == ctDefer)//�ڻ�,�ֻ�
			sprintf_s(strText, "%s %s", ContractKey.CommodityNo, ContractKey.ContractNo);
		else if (ContractKey.CommodityType == ctOption)//��Ȩ
			sprintf_s(strText, "%s %s %c%s", ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice);
		else if (ContractKey.CommodityType == ctSpreadMonth)//�ڻ��� ��������
			sprintf_s(strText, "%s %s/%s", ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.ContractNo2);
		else if (ContractKey.CommodityType == ctSpreadCommodity)//����Ʒ����
			sprintf_s(strText, "%s %s", ContractKey.CommodityNo, ContractKey.ContractNo);
		else if (ContractKey.CommodityType == ctBUL || ContractKey.CommodityType == ctBER || ContractKey.CommodityType == ctBLT || ContractKey.CommodityType == ctBRT || ContractKey.CommodityType == ctSTD || ContractKey.CommodityType == ctSTG)//��Ȩ�� ������Ȩ��ֱ�۲������Ȩ��ֱ�۲��ʽ��ϣ����ʽ��ϣ�ˮƽ����������ˮƽ��������	
			sprintf_s(strText, "%s %s %c%s/%s %c%s", ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice, ContractKey.ContractNo2, ContractKey.OptionType2, ContractKey.StrikePrice2);
		strCode = strText;
		return true;
	}
	//�����ϻ�ȡ����Ʒ����Ϣ ��С�䶯�� ���� �۸����͵�
	bool GetCommodityInf(const TCommodityKey &sCommodityKey, SOFCommodityInf & sCommodityInfo)
	{
		if (!(NULL != g_pTradeData && NULL != g_pTradeData->GetCommodity(&sCommodityKey)))
		{
			sCommodityInfo.bCanNegative = false;
			sCommodityInfo.dMiniChangePrice = 1;
			sCommodityInfo.iPrecision = 0;
			sCommodityInfo.bIfFractionPrice = false;
			return false;
		}
		const TCommodity * pCommodityInfo = g_pTradeData->GetCommodity(&sCommodityKey);//�ɲ������ݸ���
		if (0 == pCommodityInfo->LowerTick)
		{
			sCommodityInfo.bCanNegative = false;
			sCommodityInfo.dMiniChangePrice = 1;
			sCommodityInfo.iPrecision = 0;
			return true;
		}
		strncpy_s(sCommodityInfo.Sign, sCommodityKey.Sign, _TRUNCATE);
		char CommdityType = pCommodityInfo->CommodityType;
		if (ctGoods == CommdityType || ctFutures == CommdityType || ctOption == CommdityType)
			sCommodityInfo.bCanNegative = false;
		else if (ctSpreadMonth == CommdityType || ctSpreadCommodity == CommdityType ||
			ctBUL == CommdityType || ctBER == CommdityType || ctBLT == CommdityType || ctBRT == CommdityType || ctSTD == CommdityType || ctSTG == CommdityType)
			sCommodityInfo.bCanNegative = true;

		//��ƽ��Ϣ
		sCommodityInfo.cCoverMode = pCommodityInfo->CoverMode;
		switch (pCommodityInfo->LowerTick)
		{
			case 1:	//��Ӧ��С�䶯����С��
			{
				sCommodityInfo.bIfFractionPrice = false;
				sCommodityInfo.dMiniChangePrice = pCommodityInfo->UpperTick;//��С�䶯��
				sCommodityInfo.iPrecision = pCommodityInfo->Precision;
				return true;
			}
			default: //��Ӧ��С�䶯���Ƿ��� ����10/320 ����û�о��� ����û���� ���ڶ��ٲ�֪��
			{
				sCommodityInfo.bIfFractionPrice = true;
				sCommodityInfo.dMiniChangePrice = pCommodityInfo->UpperTick;//���ӱ䶯��λ ��������
				sCommodityInfo.iPrecision = pCommodityInfo->LowerTick;	//��ĸ
				break;
			}
		}
		return true;
	}
	//�����ϻ�ȡƷ����Ϣ ��С�䶯�� ���� �۸����͵�
	bool CAuxiliary::GetQuotCommodityInf(const char*code, SOFCommodityInf & sCommodityInfo)
	{
		if (!g_PlugLoad.GetQuotApi())
			return false;

		TContractKey	TradeContract;	//���׺�Լ
		memset(&TradeContract, 0, sizeof(TradeContract));
		CAuxiliary::StrToTradeContract(code, TradeContract);
		RawContractIdType typeId;
		typeId[0] = '\0';
		CAuxiliary::TradeContractToRawContractIdType(TradeContract, typeId);

		TQuoteCommodity tmpCommodity;
		memset(&tmpCommodity, 0, sizeof(TQuoteCommodity));
		sprintf_s(tmpCommodity.CommodityNo, "%s|%c|%s", TradeContract.ExchangeNo, TradeContract.CommodityType, TradeContract.CommodityNo);
		int num = g_PlugLoad.GetQuotApi()->GetCommodity(&tmpCommodity);

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
		if (tmpCommodity.PriceDeno>1)//��������
		{
			sCommodityInfo.bIfFractionPrice = true;
			sCommodityInfo.dMiniChangePrice = tmpCommodity.PriceTick;			//���ӱ䶯��λ ��������
			sCommodityInfo.iPrecision = tmpCommodity.PriceDeno;				//��ĸ
		}
		else //С������
		{
			char sPresion[100] = { 0 };
			int iPresion = 0;
			if (tmpCommodity.PriceDeno > 0.0000000001)
				sprintf_s(sPresion, "%.10f", tmpCommodity.PriceTick / tmpCommodity.PriceDeno);
			else
				sprintf_s(sPresion, "%.10f", tmpCommodity.PriceTick);
			for (int i = sizeof(sPresion) - 1; i >= 0; i--)
			{
				if (iPresion == 0 && sPresion[i] > '0')
					iPresion = i;
				if (sPresion[i] == '.'&&iPresion > i)
				{
					iPresion = iPresion - i;
					break;
				}
			}

			sCommodityInfo.bIfFractionPrice = false;
			sCommodityInfo.iPrecision = iPresion;
			sCommodityInfo.dMiniChangePrice = tmpCommodity.PriceTick;			//��С�䶯��
		}
		return true;
	}
	//��ȡ��С�䶯�۸�
	double GetMinChangePrice(const SOFCommodityInf & sCommodityInfo)
	{
		double MiniChangePrice = sCommodityInfo.dMiniChangePrice;
		if (sCommodityInfo.bIfFractionPrice)//��������
			MiniChangePrice = (sCommodityInfo.dMiniChangePrice / sCommodityInfo.iPrecision);
		return MiniChangePrice;
	}
	//�ָ��ַ���
	void StringSplit(const char* strSrc, char Separator, vector<string>& vecDest)
	{
		int nLen = strlen(strSrc);

		char tmpStr[101];
		int curLen = 0;

		for (int i = 0; i < nLen; i++)
		{
			if (strSrc[i] == Separator)
			{
				tmpStr[curLen] = 0;
				curLen = 0;
				vecDest.push_back(string(tmpStr));
				tmpStr[0] = 0;
			}
			else
			{
				tmpStr[curLen++] = strSrc[i];
			}

			if (i == nLen - 1)
			{
				tmpStr[curLen] = 0;
				vecDest.push_back(string(tmpStr));
			}
		}
	}
	//�����Ȩ�ֶ�
	bool ParseOptionContractStr(const std::string& strValue, char(&ContractNo)[11], TOptionType &OptionType, char(&StrikePrice)[11])
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
	//�����ַ���������id
	bool StrToRawContractIdType(const char *code, char(&ContractId)[51])
	{
		TContractKey	TradeContract;	//���׺�Լ
		memset(&TradeContract, 0, sizeof(TradeContract));
		StrToTradeContract(code, TradeContract);
		TradeContractToRawContractIdType(TradeContract, ContractId);
		if (g_PlugLoad.GetQuotApi())
			return  IsValidQuotContract(ContractId);
		return false;
	}
	//����id�������ַ���
	void RawContractIdTypeToStr(char(&ContractId)[51], string &str)
	{
		TContractKey	TradeContract;	//���׺�Լ
		memset(&TradeContract, 0, sizeof(TradeContract));
		RawContractIdTypeToTradeContract("", ContractId, TradeContract);
		TradeContractToStr(TradeContract, str);
	}
	//���׺�ԼתΪ����ID
	void TradeContractToRawContractIdType(const TContractKey &ContractKey, char(&ContractId)[51])
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
					ContractKey.CommodityType,
					ContractKey.CommodityNo,
					ContractKey.ContractNo);
				break;
				//��Ȩ	<Exg>|<Type>|<Root>|<YEAR><MONTH>[DAY]<OFlag><STRIKEPRICE>
			case ctOption:
				sprintf_s(ContractId, "%s|%c|%s|%s%c%s",
					ContractKey.ExchangeNo,
					ContractKey.CommodityType,
					ContractKey.CommodityNo,
					ContractKey.ContractNo,
					ContractKey.OptionType,
					ContractKey.StrikePrice);
				break;
				//�������� <Exg>|<Type>|<Root>|<YEAR><MONTH>[DAY]|<YEAR><MONTH>[DAY]
			case ctSpreadMonth:
				sprintf_s(ContractId, "%s|%c|%s|%s|%s",
					ContractKey.ExchangeNo,
					ContractKey.CommodityType,
					ContractKey.CommodityNo,
					ContractKey.ContractNo,
					ContractKey.ContractNo2);
				break;
				//��Ʒ������ <Exg>|<Type>|<Root&Root>|<YEAR><MONTH>[DAY]
			case ctSpreadCommodity:
				sprintf_s(ContractId, "%s|%c|%s|%s",
					ContractKey.ExchangeNo,
					ContractKey.CommodityType,
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
					ContractKey.CommodityType,
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
					ContractKey.CommodityType,
					ContractKey.CommodityNo);
				break;
			default:
				sprintf_s(ContractId, "Unknown Contract:%c", ContractKey.CommodityType);
				break;
		}
	}
	//����IDתΪ���׺�Լ
	void RawContractIdTypeToTradeContract(const char *szSign, const string &id, TContractKey &ContractKey)
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

		StringSplit(id.c_str(), '|', vecStr);
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
				//���ʽ����
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
	//�ж��Ƿ�����Ч�������Լ
	bool IsValidQuotContract(const  char * ContractId)
	{
		TQuoteContract tmpQuoteContract;
		memset(&tmpQuoteContract, 0, sizeof(TQuoteContract));
		strcpy_s(tmpQuoteContract.ContractId, ContractId);
		if (g_PlugLoad.GetQuotApi()->GetContract(&tmpQuoteContract))
			return true;
		return false;
	}
	//����Ƿ��н���Ա��¼��
	bool IsTraderLogined()
	{
		CDataContainer tempUser;
		g_pTradeData->GetAllLogin(tempUser);
		int iTraderNum = 0;
		for (uint i = 0; i < tempUser.size(); i++)
		{
			const TLogin *pLogin = ((TLogin*)(tempUser.at(i)));
			if (pLogin && (pLogin->Logined == bYes) && (uiTrader == pLogin->UserType))
				return true;
		}
		return false;
	}
	//�ж��Ƿ�����Ϻ�Լ
	bool IsCombContract(const char CommodityType)
	{
		if (ctSpreadMonth == CommodityType || ctSpreadCommodity == CommodityType || ctSpreadLocal == CommodityType ||
			ctBUL == CommodityType || ctBER == CommodityType || ctBLT == CommodityType || ctBRT == CommodityType ||
			ctSTD == CommodityType || ctSTG == CommodityType)
			return true;
		return false;
	}
	//��ȡȱʡĬ������
	int  GetDefaultQty(const char *szSign, int iCommodityVol, CommodityVol *pCommodityVol,
		const TContractKey &ContractKey, int iType)
	{
		TContractKey tempContractKey;
		string str1, str2;
		int iretContract = 0, iretCommodity = 0;
		bool bExContract = false, bExCommodity = false;//�Ƿ���ں�Լ��Ʒ��
		for (int i = 0; i < iCommodityVol; i++)
		{
			memset(&tempContractKey, 0, sizeof(tempContractKey));
			CommodityVol &temp = pCommodityVol[i];
			strncpy_s(tempContractKey.Sign, szSign, _TRUNCATE);
			if (!CAuxiliary::StrToTradeContract(temp.commodity, tempContractKey))//���ر�ʾƷ�ֻ��Ǻ�Լ
				continue;
			bool bContract = CAuxiliary::IsValidTradeContract(tempContractKey);
			if (bContract)//�Ǻ�Լ
			{
				CAuxiliary::FormatTradeContract(ContractKey, str1);//��Լ
				CAuxiliary::FormatTradeContract(tempContractKey, str2);
				if (str1 == str2)
				{
					bExContract = true;
					if (oOpen == iType)
						iretContract = (int)temp.vol;		//return (int)MyRound(temp.vol*m_fBL,0);
					else if (oCover == iType || oCoverT == iType)
						iretContract = (int)temp.Covervol;	//return (int)MyRound(temp.Covervol*m_fBL,0);
					break;
				}
			}
			else//��Ʒ��
			{
				sscanf_s(temp.commodity, "%s %s", tempContractKey.ExchangeNo, sizeof(tempContractKey.ExchangeNo), tempContractKey.CommodityNo, sizeof(tempContractKey.CommodityNo));
				if ((0 == strcmp(ContractKey.ExchangeNo, tempContractKey.ExchangeNo)) &&
					(0 == strcmp(ContractKey.CommodityNo, tempContractKey.CommodityNo)))//Ʒ��
				{
					bExCommodity = true;
					if (oOpen == iType)
						iretCommodity = (int)temp.vol;		//return (int)MyRound(temp.vol*m_fBL,0);
					else if (oCover == iType || oCoverT == iType)
						iretCommodity = (int)temp.Covervol;	//return (int)MyRound(temp.Covervol*m_fBL,0);
					//break;
				}
			}
		}
		if (bExContract)		return iretContract;
		if (bExCommodity)	return iretCommodity;
		return 0;
	}
	//�ж��Ƿ�����Ч�Ľ��׺�Լ
	bool IsValidTradeContract(const TContractKey & ContractKey)
	{
		const TContract * pContract = g_pTradeData->GetContract(&ContractKey);
		if (pContract)	return true;
		return false;
	}
	//��ʽ�����׺�Լ�ṹ��
	void CAuxiliary::FormatTradeContract(const TContractKey & ContractKey, string &str)
	{
		char cc[201] = { 0 };
		sprintf_s(cc, "%s&%s&%c&%s&%s&%s&%d&%s&%s&%d", ContractKey.Sign, ContractKey.ExchangeNo, ContractKey.CommodityType, ContractKey.CommodityNo, \
			ContractKey.ContractNo, ContractKey.StrikePrice, (unsigned char)ContractKey.OptionType, \
			ContractKey.ContractNo2, ContractKey.StrikePrice2, (unsigned char)ContractKey.OptionType2);
		str = cc;
	}
	//��ȡ�ʽ��˻���Ϣ
	bool GetAllUserNo(vector<CStringsVec>&vecUserNo)
	{
		//��ȡ��½
		if (!g_pTradeData) return false;
		CDataContainer tempDataCon;
		g_pTradeData->GetAllUser(tempDataCon);
		CStringsVec vecStr;
		uint num = tempDataCon.size();
		for (uint i = 0; i < num; i++)   //��ÿһ���ͻ�����
		{
			vecStr.clear();
			TUserInfo * Ref = ((TUserInfo*)(tempDataCon.at(i)));
			if (!(Ref->pLogin && (bYes == Ref->pLogin->Logined)))	continue;
			//1
			vecStr.push_back(Ref->UserNo);
			//2
			if (Ref->UserName[0] == '\0')
				vecStr.push_back(Ref->UserNo);
			else
				vecStr.push_back(Ref->UserName);
			//3 ����sign��������Ҫ��ʾ
			vecStr.push_back(Ref->pLogin->Sign);
			vecUserNo.push_back(vecStr);
		}
		return true;
	}
	//�Ƿ�����ƽ����ƽ��(�Ϻ�) ����-1������  ֻ�����ֿ���ƽ�ֵĲ���Ҫ���ô˺���
	bool IsDistinguishCoverType(const TCommodityKey*pCommoditykey)
	{
		const TCommodity * pCommodity = g_pTradeData->GetCommodity(pCommoditykey);
		if (!pCommodity)		return false;
		if (cmCoverToday == pCommodity->CoverMode)	return true;
		return false;
	}
	//API �������޹� ����0�ɹ� ���ظ���ʧ��  ��ѯ����ʧ��
	int QrySingleContractCanOpen(const TSendOrder &SendOrder)
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
	int CAuxiliary::GetCanOpenQty(char cCoverMode, const TSendOrder &SendOrder, bool bOpenMaxAutoCancel)
	{
		int uSigCanOpen = 0;

		//���ֿ�ƽ��
		bool bHaveCover = (cCoverMode == cmNone || cCoverMode == cmUnfinish) ? false : true;

		//���Ȼ������
		if (CAuxiliary::IsCombContract(SendOrder.CommodityType))//�������� 
		{
			TContractSingleKey SigCont1, SigCont2;
			CAuxiliary::CombContractToSingleContract(SendOrder, SigCont1, SigCont2);

			//������Ͽɿ�  ҲȥAPI��ѯ  Ȼ����� �������ȵ���С�ɿ����� +API���ص�  �����㷨��׼ȷ
			uSigCanOpen = QrySingleContractCanOpen(SendOrder);

			//���ֹҵ���
			const TOrderQty *p1 = g_pTradeData->GetParOrderQty(SendOrder.UserNo, &SigCont1);
			const TOrderQty *p2 = g_pTradeData->GetParOrderQty(SendOrder.UserNo, &SigCont2);

			if (bHaveCover&&bOpenMaxAutoCancel)//���ֿ�ƽ��ѡ��ƽ���Զ�����
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
				if (bHaveCover&&bOpenMaxAutoCancel)//���ֿ�ƽ��ѡ��ƽ���Զ�����
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
	//��ȡ��ƽ ����ϵͳ������Ϣ���ֲ���Ϣ 
	void CAuxiliary::GetCanCoverQty(char cCoverMode, const TContractKey &ContractKey, const char *UserNo, char Direct,
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
			CAuxiliary::GetPosCanCoverNum(UserNo, &ContractKey, Direct, bDisCover, ccTnum, cczuonum, CanCoverNum, CanCoverTNum);

			//���Ȼ������
			if (CAuxiliary::IsCombContract(ContractKey.CommodityType))//�������� 
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
	//����Ϻ�ԼתΪ�������Ⱥ�Լ
	void CombContractToSingleContract(const TContractKey &ContractKey, TContractSingleKey &SigCont1, TContractSingleKey &SigCont2)
	{
		const TCommodity *pCommodity = g_pTradeData->GetCommodity(&ContractKey);
		if (!pCommodity)	return;
		bool bFirstDirect = CombContractIsFirst(ContractKey);
		if (bFirstDirect)
		{
			//1
			strncpy_s(SigCont1.Sign, ContractKey.Sign);
			strncpy_s(SigCont1.ExchangeNo, pCommodity->RelateExchangeNo);
			SigCont1.CommodityType = pCommodity->RelateCommodityType;
			strncpy_s(SigCont1.CommodityNo, pCommodity->RelateCommodityNo);
			strncpy_s(SigCont1.ContractNo, ContractKey.ContractNo);
			strncpy_s(SigCont1.StrikePrice, ContractKey.StrikePrice);
			SigCont1.OptionType = ContractKey.OptionType;
			//2
			strncpy_s(SigCont2.Sign, ContractKey.Sign);
			strncpy_s(SigCont2.ExchangeNo, pCommodity->RelateExchangeNo2);
			SigCont2.CommodityType = pCommodity->RelateCommodityType2;
			strncpy_s(SigCont2.CommodityNo, pCommodity->RelateCommodityNo2);
			strncpy_s(SigCont2.ContractNo, ContractKey.ContractNo2);
			strncpy_s(SigCont2.StrikePrice, ContractKey.StrikePrice2);
			SigCont2.OptionType = ContractKey.OptionType2;
		}
		else//�Եڶ���Ϊ׼
		{
			//1
			strncpy_s(SigCont1.Sign, ContractKey.Sign);
			strncpy_s(SigCont1.ExchangeNo, pCommodity->RelateExchangeNo2);
			SigCont1.CommodityType = pCommodity->RelateCommodityType2;
			strncpy_s(SigCont1.CommodityNo, pCommodity->RelateCommodityNo2);
			strncpy_s(SigCont1.ContractNo, ContractKey.ContractNo2);
			strncpy_s(SigCont1.StrikePrice, ContractKey.StrikePrice2);
			SigCont1.OptionType = ContractKey.OptionType2;
			//2
			strncpy_s(SigCont2.Sign, ContractKey.Sign);
			strncpy_s(SigCont2.ExchangeNo, pCommodity->RelateExchangeNo);
			SigCont2.CommodityType = pCommodity->RelateCommodityType;
			strncpy_s(SigCont2.CommodityNo, pCommodity->RelateCommodityNo);
			strncpy_s(SigCont2.ContractNo, ContractKey.ContractNo);
			strncpy_s(SigCont2.StrikePrice, ContractKey.StrikePrice);
			SigCont2.OptionType = ContractKey.OptionType;
		}
	}
	//��Ϻ�Լ�Ƿ��Ե�һ��Ϊ׼��1�ǣ�0�Եڶ���Ϊ׼
	bool CombContractIsFirst(const TCommodityKey & CommodityKey)
	{
		const TCommodity *pCommodity = g_pTradeData->GetCommodity(&CommodityKey);
		if (!pCommodity)	return true;
		if (dfNone == pCommodity->CmbDirectFlag){}
		else if (dfFirst == pCommodity->CmbDirectFlag)
			return true;
		else if (dfSecond == pCommodity->CmbDirectFlag)
			return false;
		return true;
	}
	//�õ��ֲ֡���ƽ ���ݿͻ� ���������� ���� һ��һƽ
	void  CAuxiliary::GetPosCanCoverNum(const char *UserNo, const TContractKey * ContractKey, char Direct, bool bDisCover,
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
				CanCoverNum = (iPos>0&&iPos > QueueCount) ? (iPos - QueueCount) : 0;	//���ƽ
				CanCoverTNum = (iPosT>0&&iPosT > QueueDayCount )? (iPosT - QueueDayCount) : 0;	//���ƽ
			}
			else//�����ֿ�ƽ
			{
				CanCoverNum = (iPos>0&&iPos > QueueCount) ? (iPos - QueueCount) : 0;//��ƽ
				CanCoverTNum = 0;
			}
		}
		else//��Ͽ�ƽ �����ֽ���
		{
			TContractSingleKey SigCont1, SigCont2;
			CombContractToSingleContract(*ContractKey, SigCont1, SigCont2);
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
	//�µ�->��Էǿ���ƽ���µ������ֽ�Ϊ��������ȡ��   ������ݿ����µ� ���һ����ȡ����Ϊ�������õ�ָ���˻���ָ����Լ��ָ������ĳֲ� 
	void  CAuxiliary::GetPosNum(const char *UserNo, const TContractKey * ContractKey, char Direct,
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
				iPos = LongCount>DayLongCount?LongCount - DayLongCount:0;
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
				iPos = ShortCount>DayShortCount?ShortCount - DayShortCount:0;
			}
			else//�����ֿ�ƽ
			{
				iPosT = 0;
				iPos = ShortCount;
			}
		}
	}
	//�ֲ����� ��������ͨ����㷨��һ�� ������һ��һƽ ��ֲ�����ֻ��Ҫ��ƽ�ֵ���һ��
	void CAuxiliary::GetPositionVol(const char * UserNo, const TContractKey * ContractKey
		, uint &LongCount, uint &ShortCount, uint &DayLongCount, uint &DayShortCount)
	{
		if (UserNo == NULL || ContractKey == NULL)
		{
			g_pTradeData->GetPositionVol(NULL, NULL, LongCount, ShortCount, DayLongCount, DayShortCount);
			return;
		}
		if (!IsCombContract(ContractKey->CommodityType))
			g_pTradeData->GetPositionVol(UserNo, ContractKey, LongCount, ShortCount, DayLongCount, DayShortCount);
		else
		{
			TContractSingleKey SigCont1, SigCont2;
			CombContractToSingleContract(*ContractKey, SigCont1, SigCont2);
			uint LongCount1 = 0, ShortCount1 = 0, DayLongCount1 = 0, DayShortCount1 = 0;
			uint LongCount2 = 0, ShortCount2 = 0, DayLongCount2 = 0, DayShortCount2 = 0;
			g_pTradeData->GetPositionVol(UserNo, &SigCont1, LongCount1, ShortCount1, DayLongCount1, DayShortCount1);
			g_pTradeData->GetPositionVol(UserNo, &SigCont2, LongCount2, ShortCount2, DayLongCount2, DayShortCount2);
			//��ͷ
			LongCount = LongCount1>ShortCount2 ? ShortCount2 : LongCount1;
			DayLongCount = DayLongCount1>DayShortCount2 ? DayShortCount2 : DayLongCount1;

			//��ͷ
			ShortCount = ShortCount1>LongCount2 ? LongCount2 : ShortCount1;
			DayShortCount = DayShortCount1>DayLongCount2 ? DayLongCount2 : DayShortCount1;
		}
	}
	//�ж��Ƿ�����Ȩ��Լ
	bool IsOptionContract(const char CommodityType)
	{
		return (ctOption == CommodityType ||
			ctBUL == CommodityType || ctBER == CommodityType || ctBLT == CommodityType || ctBRT == CommodityType ||
			ctSTD == CommodityType || ctSTG == CommodityType);
	}
	//���Ƚ��׺�Լת�ַ��� Ĭ�ϲ���������
	bool SingTradeContractToStr(string & strCode, const TContractSingleKey & SingContractKey, bool bHaveExchange)
	{
		char str[201] = { 0 };
		if (SingContractKey.CommodityType == ctFutures || SingContractKey.CommodityType == ctGoods || SingContractKey.CommodityType == ctDefer)//�ڻ�,�ֻ�
		{
			if (bHaveExchange)
				sprintf_s(str,"%s %s %s %s", SingContractKey.Sign, SingContractKey.ExchangeNo, SingContractKey.CommodityNo, SingContractKey.ContractNo);
			else
				sprintf_s(str, "%s %s %s", SingContractKey.Sign, SingContractKey.CommodityNo, SingContractKey.ContractNo);
		}
		else if (SingContractKey.CommodityType == ctOption)//��Ȩ
		{
			if (bHaveExchange)
				sprintf_s(str, "%s %s %s %s %c%s", SingContractKey.Sign, SingContractKey.ExchangeNo, SingContractKey.CommodityNo, SingContractKey.ContractNo, SingContractKey.OptionType, SingContractKey.StrikePrice);
			else
				sprintf_s(str, "%s %s %s %c%s", SingContractKey.Sign, SingContractKey.CommodityNo, SingContractKey.ContractNo, SingContractKey.OptionType, SingContractKey.StrikePrice);
		}
		return true;
	}
	//��ȡswap�ֲ�
	void GetSwapPositionVol(const char * UserNo, TContractSingleKey &SigCont1, TContractSingleKey&SigCont2,PosCount&posCount)
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
	//��������
	bool IsInnerDiskExchange(const AbnormalTradeLimit& AbnormalTradeLimit, const char *szSign, const char *szExchange, Exchange_Type &ExchangeType)
	{
		if (/* (0==strcmp(szSign,AbnormalTradeLimit.ZCEsign)) && */(0 == strcmp(szExchange, AbnormalTradeLimit.ZCEExchangeNo))) {
			ExchangeType = ZCE_TYPE;
			return true;
		}
		if ( /*(0==strcmp(szSign,AbnormalTradeLimit.DCEsign)) && */(0 == strcmp(szExchange, AbnormalTradeLimit.DCEExchangeNo))) {
			ExchangeType = DCE_TYPE;
			return true;
		}
		if ( /*(0==strcmp(szSign,AbnormalTradeLimit.SHFEsign)) &&*/ (0 == strcmp(szExchange, AbnormalTradeLimit.SHFEExchangeNo))) {
			ExchangeType = SHFE_TYPE;
			return true;
		}
		if ( /*(0==strcmp(szSign,AbnormalTradeLimit.CFFEXsign)) && */(0 == strcmp(szExchange, AbnormalTradeLimit.CFFEXExchangeNo))) {
			ExchangeType = CFFEF_TYPE;
			return true;
		}
		ExchangeType = NONE_TYPE;
		return false;
	}
	TCommodityType QuoteTypeToTradeType(char cType)
	{
		switch (cType)
		{
		case COMMODITY_TYPE_NONE:				return ctNone;
		case COMMODITY_TYPE_SPOT:				return ctGoods;
		case COMMODITY_TYPE_FUTURES:			return ctFutures;
		case COMMODITY_TYPE_OPTION:				return ctOption;
		case COMMODITY_TYPE_SPREAD_MONTH:		return ctSpreadMonth;
		case COMMODITY_TYPE_SPREAD_COMMODITY:	return ctSpreadCommodity;
		case COMMODITY_TYPE_BUL:				return ctBUL;
		case COMMODITY_TYPE_BER:				return ctBER;
		case COMMODITY_TYPE_STD:				return ctSTD;
		case COMMODITY_TYPE_STG:				return ctSTG;
		case COMMODITY_TYPE_PRT:				return ctPRT;
		case COMMODITY_TYPE_BLT:				return ctBLT;
		case COMMODITY_TYPE_BRT:				return ctBRT;
		case COMMODITY_TYPE_DIRECTFOREX:		
		case COMMODITY_TYPE_DIRECTFOREXR:		return ctDirect;
		case COMMODITY_TYPE_INDIRECTFOREX:		return ctInDirect;
		case COMMODITY_TYPE_CROSSFOREX:			return ctCross;
		case COMMODITY_TYPE_INDEX:				return ctIndexNumber;
		case COMMODITY_TYPE_STOCK:				return ctStocks;
		default:
			return cType;
		}
	}
}