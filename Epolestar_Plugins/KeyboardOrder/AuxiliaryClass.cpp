#include "stdafx.h"
#include "AuxiliaryClass.h"
#include "DataContainer_Trade.h"
#include "PlugLoad.h"

//品种类型
typedef char               TapCommodityTypeType;
//无
const TapCommodityTypeType		COMMODITY_TYPE_NONE = 'N';
//现货
const TapCommodityTypeType		COMMODITY_TYPE_SPOT = 'P';
//期货
const TapCommodityTypeType		COMMODITY_TYPE_FUTURES = 'F';
//期权
const TapCommodityTypeType		COMMODITY_TYPE_OPTION = 'O';
//跨期套利
const TapCommodityTypeType		COMMODITY_TYPE_SPREAD_MONTH = 'S';
//跨品种套利
const TapCommodityTypeType		COMMODITY_TYPE_SPREAD_COMMODITY = 'M';
//看涨垂直套利
const TapCommodityTypeType		COMMODITY_TYPE_BUL = 'U';
//看跌垂直套利
const TapCommodityTypeType		COMMODITY_TYPE_BER = 'E';
//跨式套利
const TapCommodityTypeType		COMMODITY_TYPE_STD = 'D';
//宽跨式套利
const TapCommodityTypeType		COMMODITY_TYPE_STG = 'G';
//备兑组合
const TapCommodityTypeType		COMMODITY_TYPE_PRT = 'R';
//看涨水平期权
const TapCommodityTypeType		COMMODITY_TYPE_BLT = 'L';
//看跌水平期权
const TapCommodityTypeType		COMMODITY_TYPE_BRT = 'Q';
//外汇 直接汇率 USD是基础货币
const TapCommodityTypeType		COMMODITY_TYPE_DIRECTFOREX = 'X';
//外汇 直接汇率1 非USD是基础货币
const TapCommodityTypeType		COMMODITY_TYPE_DIRECTFOREXR = 'Y';
//外汇 间接汇率
const TapCommodityTypeType		COMMODITY_TYPE_INDIRECTFOREX = 'I';
//外汇 交叉汇率
const TapCommodityTypeType		COMMODITY_TYPE_CROSSFOREX = 'C';
//指数
const TapCommodityTypeType		COMMODITY_TYPE_INDEX = 'Z';
//股票
const TapCommodityTypeType		COMMODITY_TYPE_STOCK = 'T';
//中金所对锁组合
const TapCommodityTypeType		COMMODITY_TYPE_FUTURE_LOCK = 'J';
//中金所不同月份跨品种套利
const TapCommodityTypeType		COMMODITY_TYPE_SPREAD_C_COMMODITY = 'K';

extern CPlugLoad g_PlugLoad;

template<size_t _size>
inline size_t strncpy_s(char(&_dest)[_size], const char * _source) throw()
{
	return strncpy_s(_dest, _size, _source, _size - 1);
}

namespace CAuxiliary
{
	//初始化TSendOrder
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
	//登陆交易后 取遍历交易所信息 0==m_iVersionType表示内盘 1==m_iVersionType表示外盘 2==m_iVersionType表示内外盘 
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
	//字符串转交易合约
	bool StrToTradeContract(const char * szCode, TContractKey & ContractKey)
	{
		vector<string> vecstr;		//保存所有满足的
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
			else//没找到' '但有内容  类似abc;这是仍然要保存
			{
				iterator_traits<string::iterator>::distance_type dis = distance(tempit, tempstr.end());
				vecstr.push_back(tempstr);
				break;
			}
		}
		vector<string>::size_type iCount = vecstr.size();
		if (iCount < 2 || iCount>5)	return false;
		//防止连续输入多个空格
		for (vector<string>::size_type i = 0; i < iCount; i++)
		{
			if (vecstr[i].empty())	return false;
		}
		strncpy_s(ContractKey.ExchangeNo, sizeof(ContractKey.ExchangeNo), vecstr[0].c_str(), _TRUNCATE);					//交易所
		strncpy_s(ContractKey.CommodityNo, sizeof(ContractKey.CommodityNo), vecstr[1].c_str(), _TRUNCATE);				//商品
		ContractKey.OptionType = otNone;
		ContractKey.OptionType2 = otNone;
		string str;
		if (iCount == 2)
		{
			ContractKey.CommodityType = ctGoods;
			int iPs = vecstr[1].find('(');
			if (string::npos != iPs)	ContractKey.CommodityType = ctDefer;
		}
		else if (iCount == 3)														//期货或者期货组合
		{
			int iPos2 = vecstr[1].find('&');
			if (string::npos != iPos2)	//跨品种
			{
				ContractKey.CommodityType = ctSpreadCommodity;
				strncpy_s(ContractKey.ContractNo, sizeof(ContractKey.ContractNo), vecstr[2].c_str(), _TRUNCATE);
				strncpy_s(ContractKey.ContractNo2, sizeof(ContractKey.ContractNo2), vecstr[2].c_str(), _TRUNCATE);
				return true;
			}
			int isubPos = vecstr[2].find('/');
			if (string::npos != isubPos)										//组合
			{
				if (++isubPos == string::npos)		return false;
				ContractKey.CommodityType = ctSpreadMonth;
				str = vecstr[2].substr(0, --isubPos);
				strncpy_s(ContractKey.ContractNo, sizeof(ContractKey.ContractNo), str.c_str(), _TRUNCATE);
				strncpy_s(ContractKey.ContractNo2, sizeof(ContractKey.ContractNo2), vecstr[2].substr(++isubPos, vecstr[2].size() - isubPos).c_str(), _TRUNCATE);
			}
			else															//单腿
			{
				ContractKey.CommodityType = ctFutures;
				strncpy_s(ContractKey.ContractNo, sizeof(ContractKey.ContractNo), vecstr[2].c_str(), _TRUNCATE);
			}
		}
		else if (4 == iCount)													//期权
		{
			ContractKey.CommodityType = ctOption;
			strncpy_s(ContractKey.ContractNo, sizeof(ContractKey.ContractNo), vecstr[2].c_str(), _TRUNCATE);
			if (otCall == vecstr[3].at(0))									//看涨
				ContractKey.OptionType = otCall;
			else if (otPut == vecstr[3].at(0))									//看跌
				ContractKey.OptionType = otPut;
			strncpy_s(ContractKey.StrikePrice, sizeof(ContractKey.StrikePrice), vecstr[3].substr(1).c_str(), _TRUNCATE);
		}
		else if (5 == iCount)													//期权组合 目前为六种 并且第一腿 第二腿有严格的顺序
		{
			strncpy_s(ContractKey.ContractNo, sizeof(ContractKey.ContractNo), vecstr[2].c_str(), _TRUNCATE);
			if (otCall == vecstr[3].at(0))									//看涨 第一腿
				ContractKey.OptionType = otCall;
			else if (otPut == vecstr[3].at(0))									//看跌
				ContractKey.OptionType = otPut;
			vecstr[3] = vecstr[3].substr(1);
			int isubPos = vecstr[3].find('/');
			if (string::npos != isubPos)										//组合
			{
				if (++isubPos == string::npos)
					return false;
				str = vecstr[3].substr(0, --isubPos);							//
				strncpy_s(ContractKey.StrikePrice, sizeof(ContractKey.StrikePrice), str.c_str(), _TRUNCATE);				//4300/307
				strncpy_s(ContractKey.ContractNo2, sizeof(ContractKey.ContractNo2), vecstr[3].substr(++isubPos, vecstr[3].size() - isubPos).c_str(), _TRUNCATE);
			}

			if (otCall == vecstr[4].at(0))									//看涨 第二腿
				ContractKey.OptionType2 = otCall;
			else if (otPut == vecstr[4].at(0))									//看跌
				ContractKey.OptionType2 = otPut;
			strncpy_s(ContractKey.StrikePrice2, sizeof(ContractKey.StrikePrice2), vecstr[4].substr(1).c_str(), _TRUNCATE);

			//判断品种类型
			if (0 == strcmp(ContractKey.ContractNo, ContractKey.ContractNo2))	//合约月份相同
			{
				if (ContractKey.OptionType == ContractKey.OptionType2)			//同看涨或同看跌
				{
					if ((strcmp(ContractKey.StrikePrice, ContractKey.StrikePrice2) > 0) &&
						(otPut == ContractKey.OptionType))					//执行价1>执行价2 同看跌 BER 4-看跌期权垂直价差
					{
						ContractKey.CommodityType = ctBER;
					}
					else if ((strcmp(ContractKey.StrikePrice, ContractKey.StrikePrice2) < 0) &&
						(otCall == ContractKey.OptionType))			 		///执行价1<执行价2 同看涨 BUL 3-看涨期权垂直价差 
					{
						ContractKey.CommodityType = ctBUL;
					}
					else	return false;
				}
				else														//看涨看跌或者看跌看涨
				{
					if ((otCall == ContractKey.OptionType) &&
						(otPut == ContractKey.OptionType2))					//第一腿看涨 第二腿看跌
					{
						if (0 == strcmp(ContractKey.StrikePrice, ContractKey.StrikePrice2))
							ContractKey.CommodityType = ctSTD;				//STD 7-跨式组合
						else if (strcmp(ContractKey.StrikePrice, ContractKey.StrikePrice2) > 0)
							ContractKey.CommodityType = ctSTG;				//STG 8-宽跨式组合
					}
					else	return false;
				}
			}
			else															//BLT 5-看涨期权水平价差 BRT 6-看跌期权水平价差
			{
				if ((0 == strcmp(ContractKey.StrikePrice, ContractKey.StrikePrice2)) &&
					(ContractKey.OptionType == ContractKey.OptionType2))		//月份不同、执行价相同、同看涨或者同看跌
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
	//交易合约转字符串 bOnlyContract为true时只需要格式化合约部分 不需要交易所和商品
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
				//组合
				if (ctSpreadMonth == pCommodity->CommodityType || ctSpreadCommodity == pCommodity->CommodityType || ctBUL == pCommodity->CommodityType || ctBER == pCommodity->CommodityType || ctBLT == pCommodity->CommodityType || ctBRT == pCommodity->CommodityType || ctSTD == pCommodity->CommodityType || ctSTG == pCommodity->CommodityType)
					sprintf_s(strText, "%s\\%s\\%s %s", LoadRC::unicode_to_ansi(pExchange->ExchangeName).c_str(), LoadRC::unicode_to_ansi(pCommodity->CommodityName).c_str(), LoadRC::unicode_to_ansi(pContract->ContractName).c_str(), "价差");
				else
					sprintf_s(strText, "%s\\%s\\%s", LoadRC::unicode_to_ansi(pExchange->ExchangeName).c_str(), LoadRC::unicode_to_ansi(pCommodity->CommodityName).c_str(), LoadRC::unicode_to_ansi(pContract->ContractName).c_str());
			}
			strCode = strText;
			return true;
		}
		if (ContractKey.CommodityType == ctFutures || ContractKey.CommodityType == ctGoods || ContractKey.CommodityType == ctDefer)//期货,现货、现货延期
		{
			if (!bOnlyContract)
				sprintf_s(strText, "%s %s %s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo);	
			else
				sprintf_s(strText, "%s", ContractKey.ContractNo);
			
		}
		else if (ContractKey.CommodityType == ctOption)//期权
		{
			if (!bOnlyContract)
				sprintf_s(strText, "%s %s %s %c%s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice);
			else
				sprintf_s(strText, "%s %c%s", ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice);	
		}
		else if (ContractKey.CommodityType == ctSpreadMonth)//期货的 跨期套利
		{
			if (!bOnlyContract)
				sprintf_s(strText, "%s %s %s/%s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.ContractNo2);
			else
				sprintf_s(strText, "%s/%s", ContractKey.ContractNo, ContractKey.ContractNo2);	
		}
		else if (ContractKey.CommodityType == ctSpreadCommodity)//跨商品套利
		{
			if (!bOnlyContract)
				sprintf_s(strText, "%s %s %s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo);
			else
				sprintf_s(strText, "%s", ContractKey.ContractNo);
				
		}
		else if (ContractKey.CommodityType == ctBUL || ContractKey.CommodityType == ctBER || ContractKey.CommodityType == ctBLT || ContractKey.CommodityType == ctBRT || ContractKey.CommodityType == ctSTD || ContractKey.CommodityType == ctSTG)//期权的 看涨期权垂直价差，看跌期权垂直价差，跨式组合，宽跨式组合，水平看涨套利，水平看跌套利	
		{
			if (!bOnlyContract)
				sprintf_s(strText, "%s %s %s %c%s/%s %c%s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice, ContractKey.ContractNo2, ContractKey.OptionType2, ContractKey.StrikePrice2);
			else
				sprintf_s(strText, "%s %c%s/%s %c%s", ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice, ContractKey.ContractNo2, ContractKey.OptionType2, ContractKey.StrikePrice2);		
		}
		strCode = strText;
		return true;
	}
	//交易合约转字符串 只要品种和合约 不要交易所
	bool TradeContractToStr(string & strCode, const TContractKey & ContractKey)
	{
		char strText[100] = { 0 };
		const TExchange*pExchange = g_pTradeData->GetExchange(&ContractKey);
		if (ContractKey.CommodityType == ctFutures || ContractKey.CommodityType == ctGoods || ContractKey.CommodityType == ctDefer)//期货,现货
			sprintf_s(strText, "%s %s", ContractKey.CommodityNo, ContractKey.ContractNo);
		else if (ContractKey.CommodityType == ctOption)//期权
			sprintf_s(strText, "%s %s %c%s", ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice);
		else if (ContractKey.CommodityType == ctSpreadMonth)//期货的 跨期套利
			sprintf_s(strText, "%s %s/%s", ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.ContractNo2);
		else if (ContractKey.CommodityType == ctSpreadCommodity)//跨商品套利
			sprintf_s(strText, "%s %s", ContractKey.CommodityNo, ContractKey.ContractNo);
		else if (ContractKey.CommodityType == ctBUL || ContractKey.CommodityType == ctBER || ContractKey.CommodityType == ctBLT || ContractKey.CommodityType == ctBRT || ContractKey.CommodityType == ctSTD || ContractKey.CommodityType == ctSTG)//期权的 看涨期权垂直价差，看跌期权垂直价差，跨式组合，宽跨式组合，水平看涨套利，水平看跌套利	
			sprintf_s(strText, "%s %s %c%s/%s %c%s", ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice, ContractKey.ContractNo2, ContractKey.OptionType2, ContractKey.StrikePrice2);
		strCode = strText;
		return true;
	}
	//交易上获取交易品种信息 最小变动价 精度 价格类型等
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
		const TCommodity * pCommodityInfo = g_pTradeData->GetCommodity(&sCommodityKey);//由参数传递给的
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

		//开平信息
		sCommodityInfo.cCoverMode = pCommodityInfo->CoverMode;
		switch (pCommodityInfo->LowerTick)
		{
			case 1:	//对应最小变动价是小数
			{
				sCommodityInfo.bIfFractionPrice = false;
				sCommodityInfo.dMiniChangePrice = pCommodityInfo->UpperTick;//最小变动价
				sCommodityInfo.iPrecision = pCommodityInfo->Precision;
				return true;
			}
			default: //对应最小变动价是分数 形如10/320 分数没有精度 精度没意义 等于多少不知道
			{
				sCommodityInfo.bIfFractionPrice = true;
				sCommodityInfo.dMiniChangePrice = pCommodityInfo->UpperTick;//分子变动单位 当整数用
				sCommodityInfo.iPrecision = pCommodityInfo->LowerTick;	//分母
				break;
			}
		}
		return true;
	}
	//行情上获取品种信息 最小变动价 精度 价格类型等
	bool CAuxiliary::GetQuotCommodityInf(const char*code, SOFCommodityInf & sCommodityInfo)
	{
		if (!g_PlugLoad.GetQuotApi())
			return false;

		TContractKey	TradeContract;	//交易合约
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

		//正数还是负数
		char CommdityType = TradeContract.CommodityType;
		if (ctGoods == CommdityType || ctFutures == CommdityType || ctOption == CommdityType)
			sCommodityInfo.bCanNegative = false;
		else if (ctSpreadMonth == CommdityType || ctSpreadCommodity == CommdityType ||
			ctBUL == CommdityType || ctBER == CommdityType || ctBLT == CommdityType || ctBRT == CommdityType || ctSTD == CommdityType || ctSTG == CommdityType)
			sCommodityInfo.bCanNegative = true;

		//判断是分数报价还是小数报价
		if (tmpCommodity.PriceDeno>1)//分数报价
		{
			sCommodityInfo.bIfFractionPrice = true;
			sCommodityInfo.dMiniChangePrice = tmpCommodity.PriceTick;			//分子变动单位 当整数用
			sCommodityInfo.iPrecision = tmpCommodity.PriceDeno;				//分母
		}
		else //小数报价
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
			sCommodityInfo.dMiniChangePrice = tmpCommodity.PriceTick;			//最小变动价
		}
		return true;
	}
	//获取最小变动价格
	double GetMinChangePrice(const SOFCommodityInf & sCommodityInfo)
	{
		double MiniChangePrice = sCommodityInfo.dMiniChangePrice;
		if (sCommodityInfo.bIfFractionPrice)//分数报价
			MiniChangePrice = (sCommodityInfo.dMiniChangePrice / sCommodityInfo.iPrecision);
		return MiniChangePrice;
	}
	//分割字符串
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
	//填充期权字段
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
	//交易字符串到行情id
	bool StrToRawContractIdType(const char *code, char(&ContractId)[51])
	{
		TContractKey	TradeContract;	//交易合约
		memset(&TradeContract, 0, sizeof(TradeContract));
		StrToTradeContract(code, TradeContract);
		TradeContractToRawContractIdType(TradeContract, ContractId);
		if (g_PlugLoad.GetQuotApi())
			return  IsValidQuotContract(ContractId);
		return false;
	}
	//行情id到交易字符串
	void RawContractIdTypeToStr(char(&ContractId)[51], string &str)
	{
		TContractKey	TradeContract;	//交易合约
		memset(&TradeContract, 0, sizeof(TradeContract));
		RawContractIdTypeToTradeContract("", ContractId, TradeContract);
		TradeContractToStr(TradeContract, str);
	}
	//交易合约转为行情ID
	void TradeContractToRawContractIdType(const TContractKey &ContractKey, char(&ContractId)[51])
	{
		ContractId[0] = '\0';
		switch (ContractKey.CommodityType)
		{
			//期货 <Exg>|<Type>|<Root>|<YEAR><MONTH>[DAY]
			case ctFutures:
				//指数
			case ctIndexNumber:
				sprintf_s(ContractId, "%s|%c|%s|%s",
					ContractKey.ExchangeNo,
					ContractKey.CommodityType,
					ContractKey.CommodityNo,
					ContractKey.ContractNo);
				break;
				//期权	<Exg>|<Type>|<Root>|<YEAR><MONTH>[DAY]<OFlag><STRIKEPRICE>
			case ctOption:
				sprintf_s(ContractId, "%s|%c|%s|%s%c%s",
					ContractKey.ExchangeNo,
					ContractKey.CommodityType,
					ContractKey.CommodityNo,
					ContractKey.ContractNo,
					ContractKey.OptionType,
					ContractKey.StrikePrice);
				break;
				//跨期套利 <Exg>|<Type>|<Root>|<YEAR><MONTH>[DAY]|<YEAR><MONTH>[DAY]
			case ctSpreadMonth:
				sprintf_s(ContractId, "%s|%c|%s|%s|%s",
					ContractKey.ExchangeNo,
					ContractKey.CommodityType,
					ContractKey.CommodityNo,
					ContractKey.ContractNo,
					ContractKey.ContractNo2);
				break;
				//跨品种套利 <Exg>|<Type>|<Root&Root>|<YEAR><MONTH>[DAY]
			case ctSpreadCommodity:
				sprintf_s(ContractId, "%s|%c|%s|%s",
					ContractKey.ExchangeNo,
					ContractKey.CommodityType,
					ContractKey.CommodityNo,
					ContractKey.ContractNo);
				break;
				//看涨垂直套利
			case ctBUL:
				//看跌垂直套利
			case ctBER:
				//跨式套利
			case ctSTD:
				//宽跨式套利
			case ctSTG:
				//期权套利	<Exg>|<Type>|<Root[&Root]>|<YEAR><MONTH>[DAY]<OFlag><STRIKEPRICE>|<YEAR><MONTH>[DAY]<OFlag><STRIKEPRICE>
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
				//备兑组合
			case ctPRT:
				break;
				//现货
			case ctGoods:
				//外汇——直接汇率
			case ctDirect:
				//外汇——间接汇率
			case ctInDirect:
				//外汇——交叉汇率
			case ctCross:
				//股票
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
	//行情ID转为交易合约
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
		//按类型填充字段
		switch (ContractKey.CommodityType)
		{
			//期货 <Exg>|<Type>|<Root>|<YEAR><MONTH>[DAY]
			case ctFutures:
				//指数
			case ctIndexNumber:
				if (vecStr.size() > 3)
					strncpy_s(ContractKey.ContractNo, vecStr[3].c_str(), sizeof(ContractKey.ContractNo) - 1);
				break;
				//期权	<Exg>|<Type>|<Root>|<YEAR><MONTH>[DAY]<OFlag><STRIKEPRICE>
			case ctOption:
				if (vecStr.size() > 3)
					ParseOptionContractStr(vecStr[3], ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice);
				break;
				//跨期套利 <Exg>|<Type>|<Root>|<YEAR><MONTH>[DAY]|<YEAR><MONTH>[DAY]
			case ctSpreadMonth:
				if (vecStr.size() > 4)
				{
					strncpy_s(ContractKey.ContractNo, vecStr[3].c_str(), sizeof(ContractKey.ContractNo) - 1);
					strncpy_s(ContractKey.ContractNo2, vecStr[4].c_str(), sizeof(ContractKey.ContractNo2) - 1);
				}
				break;
				//跨品种套利 <Exg>|<Type>|<Root&Root>|<YEAR><MONTH>[DAY]
			case ctSpreadCommodity:
				if (vecStr.size() > 3)
				{
					strncpy_s(ContractKey.ContractNo, vecStr[3].c_str(), sizeof(ContractKey.ContractNo) - 1);
					strncpy_s(ContractKey.ContractNo2, vecStr[3].c_str(), sizeof(ContractKey.ContractNo2) - 1);
				}
				break;
				//看涨垂直套利
			case ctBUL:
				//看跌垂直套利
			case ctBER:
				//跨式套利
			case ctSTD:
				//宽跨式套利
			case ctSTG:
				//期权套利	<Exg>|<Type>|<Root[&Root]>|<YEAR><MONTH>[DAY]<OFlag><STRIKEPRICE>|<YEAR><MONTH>[DAY]<OFlag><STRIKEPRICE>
				if (vecStr.size() > 4)
				{
					ParseOptionContractStr(vecStr[3], ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice);
					ParseOptionContractStr(vecStr[4], ContractKey.ContractNo2, ContractKey.OptionType2, ContractKey.StrikePrice2);
				}
				break;
				//备兑组合
			case ctPRT:
				//现货
			case ctGoods:
				//外汇——直接汇率
			case ctDirect:
				//外汇——间接汇率
			case ctInDirect:
				//外汇——交叉汇率
			case ctCross:
				//股票
			case ctStocks:
			case ctStatics:
				break;
			default:
				break;
		}
	}
	//判断是否是有效的行情合约
	bool IsValidQuotContract(const  char * ContractId)
	{
		TQuoteContract tmpQuoteContract;
		memset(&tmpQuoteContract, 0, sizeof(TQuoteContract));
		strcpy_s(tmpQuoteContract.ContractId, ContractId);
		if (g_PlugLoad.GetQuotApi()->GetContract(&tmpQuoteContract))
			return true;
		return false;
	}
	//检查是否有交易员登录过
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
	//判断是否是组合合约
	bool IsCombContract(const char CommodityType)
	{
		if (ctSpreadMonth == CommodityType || ctSpreadCommodity == CommodityType || ctSpreadLocal == CommodityType ||
			ctBUL == CommodityType || ctBER == CommodityType || ctBLT == CommodityType || ctBRT == CommodityType ||
			ctSTD == CommodityType || ctSTG == CommodityType)
			return true;
		return false;
	}
	//获取缺省默认数量
	int  GetDefaultQty(const char *szSign, int iCommodityVol, CommodityVol *pCommodityVol,
		const TContractKey &ContractKey, int iType)
	{
		TContractKey tempContractKey;
		string str1, str2;
		int iretContract = 0, iretCommodity = 0;
		bool bExContract = false, bExCommodity = false;//是否存在合约、品种
		for (int i = 0; i < iCommodityVol; i++)
		{
			memset(&tempContractKey, 0, sizeof(tempContractKey));
			CommodityVol &temp = pCommodityVol[i];
			strncpy_s(tempContractKey.Sign, szSign, _TRUNCATE);
			if (!CAuxiliary::StrToTradeContract(temp.commodity, tempContractKey))//返回表示品种还是合约
				continue;
			bool bContract = CAuxiliary::IsValidTradeContract(tempContractKey);
			if (bContract)//是合约
			{
				CAuxiliary::FormatTradeContract(ContractKey, str1);//合约
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
			else//是品种
			{
				sscanf_s(temp.commodity, "%s %s", tempContractKey.ExchangeNo, sizeof(tempContractKey.ExchangeNo), tempContractKey.CommodityNo, sizeof(tempContractKey.CommodityNo));
				if ((0 == strcmp(ContractKey.ExchangeNo, tempContractKey.ExchangeNo)) &&
					(0 == strcmp(ContractKey.CommodityNo, tempContractKey.CommodityNo)))//品种
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
	//判断是否是有效的交易合约
	bool IsValidTradeContract(const TContractKey & ContractKey)
	{
		const TContract * pContract = g_pTradeData->GetContract(&ContractKey);
		if (pContract)	return true;
		return false;
	}
	//格式化交易合约结构体
	void CAuxiliary::FormatTradeContract(const TContractKey & ContractKey, string &str)
	{
		char cc[201] = { 0 };
		sprintf_s(cc, "%s&%s&%c&%s&%s&%s&%d&%s&%s&%d", ContractKey.Sign, ContractKey.ExchangeNo, ContractKey.CommodityType, ContractKey.CommodityNo, \
			ContractKey.ContractNo, ContractKey.StrikePrice, (unsigned char)ContractKey.OptionType, \
			ContractKey.ContractNo2, ContractKey.StrikePrice2, (unsigned char)ContractKey.OptionType2);
		str = cc;
	}
	//获取资金账户信息
	bool GetAllUserNo(vector<CStringsVec>&vecUserNo)
	{
		//获取登陆
		if (!g_pTradeData) return false;
		CDataContainer tempDataCon;
		g_pTradeData->GetAllUser(tempDataCon);
		CStringsVec vecStr;
		uint num = tempDataCon.size();
		for (uint i = 0; i < num; i++)   //对每一个客户处理
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
			//3 保存sign但并不需要显示
			vecStr.push_back(Ref->pLogin->Sign);
			vecUserNo.push_back(vecStr);
		}
		return true;
	}
	//是否区分平今还是平昨(上海) 返回-1有问题  只有区分开仓平仓的才需要调用此函数
	bool IsDistinguishCoverType(const TCommodityKey*pCommoditykey)
	{
		const TCommodity * pCommodity = g_pTradeData->GetCommodity(pCommoditykey);
		if (!pCommodity)		return false;
		if (cmCoverToday == pCommodity->CoverMode)	return true;
		return false;
	}
	//API 与配置无关 返回0成功 返回负号失败  查询参数失败
	int QrySingleContractCanOpen(const TSendOrder &SendOrder)
	{
		//应该根据Direct来计算可开数量
		int canqty = 0;
		if (g_pTradeData)
		{
			int iret = g_pTradeData->GetTradableQty(SendOrder, canqty);
			if (0 != iret)
				canqty = -1;
		}

		return canqty;
	}
	//获取可开 根据系统配置信息、持仓信息 
	int CAuxiliary::GetCanOpenQty(char cCoverMode, const TSendOrder &SendOrder, bool bOpenMaxAutoCancel)
	{
		int uSigCanOpen = 0;

		//区分开平吗
		bool bHaveCover = (cCoverMode == cmNone || cCoverMode == cmUnfinish) ? false : true;

		//单腿还是组合
		if (CAuxiliary::IsCombContract(SendOrder.CommodityType))//如果是组合 
		{
			TContractSingleKey SigCont1, SigCont2;
			CAuxiliary::CombContractToSingleContract(SendOrder, SigCont1, SigCont2);

			//计算组合可开  也去API查询  然后根据 两个单腿的最小可开数量 +API返回的  这种算法不准确
			uSigCanOpen = QrySingleContractCanOpen(SendOrder);

			//开仓挂单量
			const TOrderQty *p1 = g_pTradeData->GetParOrderQty(SendOrder.UserNo, &SigCont1);
			const TOrderQty *p2 = g_pTradeData->GetParOrderQty(SendOrder.UserNo, &SigCont2);

			if (bHaveCover&&bOpenMaxAutoCancel)//区分开平且选中平满自动撤单
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
			//API查询可开数量
			uSigCanOpen = QrySingleContractCanOpen(SendOrder);
			if (uSigCanOpen < 0)
				return -1;
			//挂单量
			const TOrderQty *pOrderQty = g_pTradeData->GetParOrderQty(SendOrder.UserNo, &SendOrder);

			//单腿
			if (pOrderQty)
			{
				if (bHaveCover&&bOpenMaxAutoCancel)//区分开平且选中平满自动撤单
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
	//获取可平 根据系统配置信息、持仓信息 
	void CAuxiliary::GetCanCoverQty(char cCoverMode, const TContractKey &ContractKey, const char *UserNo, char Direct,
		CanCoverQty& CanCoverQty, bool bLiquidateAutoCancel)
	{
		//如何处理竖向下单自动开平功能？

		uint ccTnum = 0, cczuonum = 0;		//今持仓，昨持仓
		uint CanCoverNum = 0, CanCoverTNum = 0;//昨可平，今可平

		//区分开平吗
		bool bHaveCover = (cCoverMode == cmNone || cCoverMode == cmUnfinish) ? false : true;

		if (bHaveCover)
		{
			//获取持仓和可平数量
			bool bDisCover = (cCoverMode == cmCoverToday) ? 1 : 0;
			CAuxiliary::GetPosCanCoverNum(UserNo, &ContractKey, Direct, bDisCover, ccTnum, cczuonum, CanCoverNum, CanCoverTNum);

			//单腿还是组合
			if (CAuxiliary::IsCombContract(ContractKey.CommodityType))//如果是组合 
			{
				//区分今昨的交易所没有组合 此假设不存在时代码则有问题
				if (bLiquidateAutoCancel)
					CanCoverQty.uCanCoverQty = cczuonum;
				else
					CanCoverQty.uCanCoverQty = CanCoverNum;
			}
			else
			{
				//单腿 m_SysGeneralOrderSetting.bAutoSelectPJPZ;
				//平今平昨自适应 与此有关系吗？有关 在调用此函数的地方自己处理 因为针对上海的显示不一样 上海显示可平昨、可平今
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
				else//不区分今昨
				{
					if (bLiquidateAutoCancel)
						CanCoverQty.uCanCoverQty = cczuonum;
					else
						CanCoverQty.uCanCoverQty = CanCoverNum;
				}
			}
		}
		else//外盘
		{
			//获取持仓数量 单腿还是组合都一样
			uint uCanCover = 0;
			CanCoverQty.uCanCoverQty = uCanCover;
		}
	}
	//把组合合约转为两个单腿合约
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
		else//以第二腿为准
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
	//组合合约是否以第一腿为准，1是，0以第二腿为准
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
	//得到持仓、可平 根据客户 、内盘所用 互换 一开一平
	void  CAuxiliary::GetPosCanCoverNum(const char *UserNo, const TContractKey * ContractKey, char Direct, bool bDisCover,
		uint &iPosT, uint &iPos, uint &CanCoverNum, uint&CanCoverTNum)
	{
		//根据某些参数运算出来是平几张 判断合约反方向的持仓
		char DifDirect = dNone;
		DifDirect = (Direct == dBuy) ? dSell : dBuy;
		GetPosNum(UserNo, ContractKey, DifDirect, iPosT, iPos);

		//得到可平 单腿和组合不同
		if (!IsCombContract(ContractKey->CommodityType))
		{
			const TOrderQty *pOrderQty = g_pTradeData->GetParOrderQty(UserNo, ContractKey);
			uint  QueueCount = 0, QueueDayCount = 0;//挂单量
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

			//计算可平数目
			if (bDisCover)//区分开平
			{
				CanCoverNum = (iPos>0&&iPos > QueueCount) ? (iPos - QueueCount) : 0;	//昨可平
				CanCoverTNum = (iPosT>0&&iPosT > QueueDayCount )? (iPosT - QueueDayCount) : 0;	//今可平
			}
			else//不区分开平
			{
				CanCoverNum = (iPos>0&&iPos > QueueCount) ? (iPos - QueueCount) : 0;//可平
				CanCoverTNum = 0;
			}
		}
		else//组合可平 不区分今昨
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
			if (Direct == dBuy)//第一腿持卖减去第一腿买平挂单A 得出可平 第二腿持买减去第二腿卖平挂单B min(A,B)得到组合买入可平
				CanCoverNum = min(ShortCount1 - (pOrderQty1 ? (pOrderQty1->BuyCoverQty + pOrderQty1->BuyCoverDayQty) : 0), LongCount2 - (pOrderQty2 ? (pOrderQty2->SellCoverQty + pOrderQty2->SellCoverDayQty) : 0));
			else//第一腿持买减去第一腿卖平挂单A 第二腿持卖减去第二腿买平挂单B min(A,B)得到组合卖出可平
				CanCoverNum = min(LongCount1 - (pOrderQty1 ? (pOrderQty1->SellCoverQty + pOrderQty1->SellCoverDayQty) : 0), ShortCount2 - (pOrderQty2 ? (pOrderQty2->BuyCoverQty + pOrderQty2->BuyCoverDayQty) : 0));
		}
	}
	//下单->针对非快速平仓下单（开仓较为简单无需提取）   若想兼容快速下单 需进一步提取（较为繁琐）得到指定账户、指定合约、指定方向的持仓 
	void  CAuxiliary::GetPosNum(const char *UserNo, const TContractKey * ContractKey, char Direct,
		uint &iPosT, uint &iPos)
	{
		//是否区分今昨
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
			else//不区分
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
			else//不区分开平
			{
				iPosT = 0;
				iPos = ShortCount;
			}
		}
	}
	//持仓数量 互换跟普通组合算法不一样 互换是一开一平 算持仓数量只需要算平仓的那一腿
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
			//多头
			LongCount = LongCount1>ShortCount2 ? ShortCount2 : LongCount1;
			DayLongCount = DayLongCount1>DayShortCount2 ? DayShortCount2 : DayLongCount1;

			//空头
			ShortCount = ShortCount1>LongCount2 ? LongCount2 : ShortCount1;
			DayShortCount = DayShortCount1>DayLongCount2 ? DayLongCount2 : DayShortCount1;
		}
	}
	//判断是否是期权合约
	bool IsOptionContract(const char CommodityType)
	{
		return (ctOption == CommodityType ||
			ctBUL == CommodityType || ctBER == CommodityType || ctBLT == CommodityType || ctBRT == CommodityType ||
			ctSTD == CommodityType || ctSTG == CommodityType);
	}
	//单腿交易合约转字符串 默认不带交易所
	bool SingTradeContractToStr(string & strCode, const TContractSingleKey & SingContractKey, bool bHaveExchange)
	{
		char str[201] = { 0 };
		if (SingContractKey.CommodityType == ctFutures || SingContractKey.CommodityType == ctGoods || SingContractKey.CommodityType == ctDefer)//期货,现货
		{
			if (bHaveExchange)
				sprintf_s(str,"%s %s %s %s", SingContractKey.Sign, SingContractKey.ExchangeNo, SingContractKey.CommodityNo, SingContractKey.ContractNo);
			else
				sprintf_s(str, "%s %s %s", SingContractKey.Sign, SingContractKey.CommodityNo, SingContractKey.ContractNo);
		}
		else if (SingContractKey.CommodityType == ctOption)//期权
		{
			if (bHaveExchange)
				sprintf_s(str, "%s %s %s %s %c%s", SingContractKey.Sign, SingContractKey.ExchangeNo, SingContractKey.CommodityNo, SingContractKey.ContractNo, SingContractKey.OptionType, SingContractKey.StrikePrice);
			else
				sprintf_s(str, "%s %s %s %c%s", SingContractKey.Sign, SingContractKey.CommodityNo, SingContractKey.ContractNo, SingContractKey.OptionType, SingContractKey.StrikePrice);
		}
		return true;
	}
	//获取swap持仓
	void GetSwapPositionVol(const char * UserNo, TContractSingleKey &SigCont1, TContractSingleKey&SigCont2,PosCount&posCount)
	{
		if (UserNo == NULL)
			return;
		uint LongCount1 = 0, ShortCount1 = 0, DayLongCount1 = 0, DayShortCount1 = 0;
		uint LongCount2 = 0, ShortCount2 = 0, DayLongCount2 = 0, DayShortCount2 = 0;
		g_pTradeData->GetPositionVol(UserNo, &SigCont1, LongCount1, ShortCount1, DayLongCount1, DayShortCount1);
		g_pTradeData->GetPositionVol(UserNo, &SigCont2, LongCount2, ShortCount2, DayLongCount2, DayShortCount2);

		//买入开仓 第一腿买入开仓(持第一腿买) 
		posCount.BuyOpenCount = LongCount1;
		//买入平仓 第一腿买入平仓 第二腿卖出开仓(持第二腿卖)
		posCount.BuyCoverCount = ShortCount2;
		//卖出开仓 第一腿卖出开仓(持第一腿卖) 
		posCount.SellOpenCount = ShortCount1;
		//卖出平仓 第一腿卖出平仓 第二腿买入开仓(持第二腿买)
		posCount.SellCoverCount = LongCount2;
	}
	//是内盘吗
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