#include "PreInclude.h"

TFrmData::TFrmData()
{
	m_szUserNo[0] = '\0';
	m_dProfit = 0.0;
	m_iTimeRange = 8;
}
TFrmData::~TFrmData()
{

}
void TFrmData::GetData()
{
	m_vecContractNo.clear();
	m_vecTradeData.clear();
	m_mapPositionSub.clear();
	m_vecMatchDateTime.clear();
	m_dFee = 0;
	m_dProfit = 0;
	DealData();
}

bool TFrmData::DealData()
{
	TDataContainer matchs;
	TDataContainer moneys;
	TDataContainer users;
	g_pTradeData->GetAllMoney(moneys);
	g_pTradeData->GetAllUser(users);
	if (!GetMatch(matchs) || moneys.size() == 0 || users.size() == 0)
		return false;
	TMoney *pMoney = (TMoney*)moneys.at(0);
	strcpy_s(m_szUserNo, ((TUserInfo*)users.at(0))->UserNo);
	double dNowMoney = pMoney->Available;
	Sort(matchs);
	double   tempData = 0.0;
	vector<TMatch*>tempMatchs;
	for (UINT i = 0; i < matchs.size(); i++)
	{
		tempMatchs.push_back((TMatch*)matchs.at(i));
	}
	DealMatchDateTime(tempMatchs);
	if (!DealTimeRange(tempMatchs, dNowMoney))
		return false;
	GetContractNo(tempMatchs);

	vector<long long> tempTradeData;
	tempTradeData.push_back(dNowMoney + 0.5);
	m_dFee = 0.0;

	for (UINT i = 0; i < tempMatchs.size(); i++)  //倒着推算钱数
	{
		m_dFee += tempMatchs[i]->MatchFee;
		if (tempMatchs[i]->Offset == oCover || tempMatchs[i]->Offset == oCoverT)//如果是平仓，看它们的流水号是否一样，如果一样，说明是同一笔平仓
		{
			tempData += tempMatchs[i]->CoverProfit - tempMatchs[i]->MatchFee;
			m_dProfit += tempMatchs[i]->CoverProfit;     //计算盈亏

			while ((i + 1) < tempMatchs.size() && strcmp(tempMatchs[i]->OrderNo, tempMatchs[i + 1]->OrderNo) == 0)
			{
				tempData += tempMatchs[i + 1]->CoverProfit - tempMatchs[i + 1]->MatchFee;
				i++;
				m_dProfit += tempMatchs[i]->CoverProfit; //计算盈亏
				m_dFee += tempMatchs[i]->MatchFee;
			}
			tempTradeData.push_back(dNowMoney - tempData + 0.5);
		}
		else if (tempMatchs[i]->Offset == oOpen)//如果是开仓，则需要查保证金
		{
			dNowMoney += GetPositionSubMoney(tempMatchs[i]);
		}
	}//for
	for (UINT i = tempTradeData.size(); i > 0; i--)
	{
		m_vecTradeData.push_back(tempTradeData[i - 1]);
	}
	return true;
}

bool TFrmData::GetMatch(TDataContainer &matchs)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	g_pTradeData->GetAllMatch(matchs);
	if (st.wHour > 15 && st.wHour < 21)
	{
		if (matchs.size() == 0)
		{
			matchs.clear();
			if (g_pTradeStatistic)
				g_pTradeStatistic->Get_Match_Data_File(matchs);
		}
	}
	return !(matchs.size() == 0);
}

void TFrmData::GetContractNo(vector<TMatch*> &vMatchs)
{
	//	char szText[50] = { 0 };
	char cbText[50] = { 0 };

	for (UINT i = 0; i < vMatchs.size(); i++)
	{
		TMatch * pMatch = vMatchs[i];
		sprintf_s(cbText, "%s%s", pMatch->CommodityNo, pMatch->ContractNo);
		if (pMatch->Offset != oOpen && find(m_vecContractNo.begin(), m_vecContractNo.end(), cbText) == m_vecContractNo.end())
		{
			m_vecContractNo.push_back(cbText);
		}
	}
}
bool CompareIntFunc1(const void* x, const void* y)
{
	return ((TMatch*)x)->StreamID > ((TMatch*)y)->StreamID;
}
void TFrmData::Sort(TDataContainer &data)
{
	vector < const void* > *tempData = data.GetData();
	sort(tempData->begin(), tempData->end(), CompareIntFunc1);
}

bool TFrmData::DealTimeRange(vector<TMatch*> &matchs, double &dNowMoney)
{
	char beginTime[21] = { 0 };  //开始时间
	char endTime[21] = { 0 };  //结束时间	
	if (!SetRangeTime(beginTime, endTime))
		return false;
	if (m_iTimeRange==periodOne||m_iTimeRange==periodTwo||m_iTimeRange==night)
		DealFriday(beginTime, endTime);
	if (m_iTimeRange >= periodOne && m_iTimeRange <= night)
	{
		int i = 0;
		int iBegin = -1;
		int iEnd = -1;
		bool bFindEnd = false;
		while (i <m_vecMatchDateTime.size() /*matchs.size()*/)//寻找开始位置
		{
			//	if ((matchs[i]->Offset== oCover ||matchs[i]->Offset==oCoverT))
			//	{
			if (!bFindEnd && (strcmp(endTime, m_vecMatchDateTime[i].c_str()/*matchs[i]->MatchDateTime*/) >= 0) && (strcmp(beginTime, m_vecMatchDateTime[i].c_str()/*matchs[i]->MatchDateTime*/) < 0)) //先找结束时间
			{
				iBegin = i;
				bFindEnd = true;
			}
			if (bFindEnd && strcmp(beginTime, m_vecMatchDateTime[i].c_str()/*matchs[i]->MatchDateTime*/) >= 0) //再找开始时间
			{
				iEnd = i;
				break;
			}
			//	}
			i++;
		}
		if (iBegin == -1)  //没找到则直接返回
			return false;
		if (iEnd == -1)  //没有找到，说明都符合条件
		{
			if (iBegin == 0)//说明全都在时间条件范围内
				return true;
			else
				iEnd = matchs.size();
		}
		for (UINT i = 0; i < iBegin; i++) //iBegin>0,需要计算到之前的money
		{
			if (matchs[i]->Offset == oCoverT || matchs[i]->Offset == oCover)
			{
				dNowMoney += matchs[i]->MatchFee - matchs[i]->CoverProfit;
			}
			else if (matchs[i]->Offset == oOpen)
			{
				dNowMoney += GetPositionSubMoney(matchs[i]);
			}
		}
		vector<TMatch*> tempMatchs;
		for (UINT i = iBegin; i < iEnd; i++)
		{
			tempMatchs.push_back(matchs[i]);
		}
		matchs.clear();
		matchs = tempMatchs;
		return true;
	}
	return true;
}

double TFrmData::GetPositionSubMoney(TMatch *pMatch)
{
	const TCommodity *pCommodity = g_pTradeData->GetCommodity(pMatch);
	if (!pCommodity) return pMatch->MatchFee;

	char positionPrice[50] = { 0 };
	sprintf_s(positionPrice, pCommodity->PriceFormat, pMatch->MatchPrice);

	char szKey[100] = { 0 };
	sprintf_s(szKey, "%s,%s,%s,%d,%s", pMatch->ExchangeNo, pMatch->CommodityNo, pMatch->ContractNo, pMatch->Direct, positionPrice);

	auto iter = m_mapPositionSub.find(szKey);
	if (iter == m_mapPositionSub.end())
	{
		const TPosition *pPosition = g_pTradeData->GetPositionSub(m_szUserNo, pMatch, pMatch->Direct, positionPrice);
		double dMoney = pMatch->MatchFee;
		if (pPosition)
		{
			m_mapPositionSub.insert(make_pair(szKey, '0'));
			dMoney += (pPosition->Deposit - pPosition->FloatProfitTBT)*pMatch->MatchQty / pPosition->PositionQty;
		}
		return dMoney;
	}
	else
		return pMatch->MatchFee;
}

bool TFrmData::SetRangeTime(char *strBeginTime, char *strEndTime, int length)
{
	if (m_iTimeRange == allday)
	{
		return true;
	}
	char beginTime[][9] = { "21:00:00", "22:00:00", "09:00:00", "10:15:00", "13:00:00", "14:15:00", "09:00:00", "21:00:00" };
	char endTime[][9] = { "22:00:00", "02:30:00", "10:15:00", "11:30:00", "14:15:00", "15:15:00", "15:15:00", "02:30:00" };
	SYSTEMTIME stToday = { 0 };
	SYSTEMTIME stYesterday = { 0 };
	GetLocalTime(&stToday);
	GetYesterday(stToday, stYesterday);
	if (stToday.wHour > 20 && stToday.wHour < 24)
	{
		if (m_iTimeRange == periodThree || m_iTimeRange == periodFour || m_iTimeRange == periodFive || m_iTimeRange == periodSix || m_iTimeRange == daytime)
			return false;
		else if (m_iTimeRange == periodOne || m_iTimeRange == night || m_iTimeRange == periodTwo)
		{
			sprintf_s(strBeginTime, length, "%04d-%02d-%02d %s", stToday.wYear, stToday.wMonth, stToday.wDay, beginTime[m_iTimeRange]);
			if (m_iTimeRange == periodOne)
				sprintf_s(strEndTime, length, "%04d-%02d-%02d %s", stToday.wYear, stToday.wMonth, stToday.wDay, endTime[m_iTimeRange]);
			else
				sprintf_s(strEndTime, length, "%04d-%02d-%02d 23:59:59", stToday.wYear, stToday.wMonth, stToday.wDay);
			return true;
		}
	}
	else
	{
		switch (m_iTimeRange)
		{
		case periodThree:      //当天时间
		case periodFour:
		case periodFive:
		case periodSix:
		case daytime:
			sprintf_s(strBeginTime, length, "%04d-%02d-%02d %s", stToday.wYear, stToday.wMonth, stToday.wDay, beginTime[m_iTimeRange]);
			sprintf_s(strEndTime, length, "%04d-%02d-%02d %s", stToday.wYear, stToday.wMonth, stToday.wDay, endTime[m_iTimeRange]);
			break;
		case periodOne:
			sprintf_s(strBeginTime, length, "%04d-%02d-%02d %s", stYesterday.wYear, stYesterday.wMonth, stYesterday.wDay, beginTime[m_iTimeRange]);
			sprintf_s(strEndTime, length, "%04d-%02d-%02d %s", stYesterday.wYear, stYesterday.wMonth, stYesterday.wDay, endTime[m_iTimeRange]);
			break;
		case periodTwo:       //需要昨天时间
		case night:
			sprintf_s(strBeginTime, length, "%04d-%02d-%02d %s", stYesterday.wYear, stYesterday.wMonth, stYesterday.wDay, beginTime[m_iTimeRange]);
			sprintf_s(strEndTime, length, "%04d-%02d-%02d %s", stToday.wYear, stToday.wMonth, stToday.wDay, endTime[m_iTimeRange]);
			break;
		}
		return true;
	}
	return false;
}

//bool TFrmData::SetRangeTime(char *strBeginTime, char *strEndTime, int length /* = 21 */)
//{
//	if (m_iTimeRange == allday)
//	{
//		return true;
//	}
//	char beginTime[][9] = { "21:00:00", "22:00:00", "09:00:00", "10:15:00", "13:00:00", "14:15:00", "09:00:00", "21:00:00" };
//	char endTime[][9] = { "22:00:00", "02:30:00", "10:15:00", "11:30:00", "14:15:00", "15:15:00", "15:15:00", "02:30:00" };
//	strcpy_s(strBeginTime, length, beginTime[m_iTimeRange]);
//	strcpy_s(strEndTime, length, endTime[m_iTimeRange]);
//}
bool TFrmData::SetRangeTime(int iBegin, int iEnd)
{
	if (m_iTimeRange == allday)
	{
		return true;
	}
	int beginTime[9] = { -30000, -200000 ,90000 , 101500, 130000,141500, 90000, -30000 };
	int endTime[9] = { -20000, 23000, 101500, 113000, 141500, 151500, 151500, 23000 };
	iBegin = beginTime[m_iTimeRange];
	iEnd = endTime[m_iTimeRange];
}
bool TFrmData::DealFriday(char *strBeginTime, char *strEndTime, int length)
{
	SYSTEMTIME stToday = { 0 };
	GetLocalTime(&stToday);

	if (stToday.wDayOfWeek != 6 && stToday.wDayOfWeek != 0 && stToday.wDayOfWeek != 1)
		return true;

	time_t tToday = time(NULL);
	tm stDateY = { 0 };
	tm stDateYY = { 0 };
	tm stDateYYY = { 0 };
	int iOneDaySecond = 86400;
	time_t tTemp = tToday - iOneDaySecond;
	localtime_s(&stDateY, &tTemp);
	tTemp -= iOneDaySecond;
	localtime_s(&stDateYY, &tTemp);
	tTemp -= iOneDaySecond;
	localtime_s(&stDateYYY, &tTemp);
	if (stToday.wDayOfWeek == 6) //周六
	{
		if (stToday.wHour > 20) //九点后
		{
			strBeginTime[8] = stDateY.tm_mday/10 + '0';
			strBeginTime[9] = stDateY.tm_mday%10 + '0';
			if (m_iTimeRange == periodOne)
			{
				strEndTime[8] = stDateY.tm_mday / 10 + '0';
				strEndTime[9] = stDateY.tm_mday % 10 + '0';
			}
		}
	}
	else if (stToday.wDayOfWeek == 0)//周日全天
	{
		strBeginTime[8] = stDateYY.tm_mday / 10 + '0';
		strBeginTime[9] = stDateYY.tm_mday % 10 + '0';
		if (m_iTimeRange == periodOne)
		{
			strEndTime[8] = stDateYY.tm_mday / 10 + '0';
			strEndTime[9] = stDateYY.tm_mday % 10 + '0';
		}
	}
	else if (stToday.wDayOfWeek == 1)//周一
	{
		if (stToday.wHour < 21)
		{
			strBeginTime[8] = stDateYYY.tm_mday / 10 + '0';
			strBeginTime[9] = stDateYYY.tm_mday % 10 + '0';
			if (m_iTimeRange == periodOne)
			{
				strEndTime[8] = stDateYYY.tm_mday / 10 + '0';
				strEndTime[9] = stDateYYY.tm_mday % 10 + '0';
			}
		}
	}
	return true;
}
void TFrmData::GetYesterday(SYSTEMTIME &today, SYSTEMTIME &yesterday)
{
	yesterday = today;
	if (today.wDay > 1)
	{
		yesterday.wDay -= 1;
	}
	else               //如果是一号则需要进行特殊处理
	{
		yesterday.wMonth -= 1;
		if (today.wMonth == 2 || today.wMonth == 4 || today.wMonth == 6 || today.wMonth == 8 || today.wMonth == 9 || today.wMonth == 11)
		{
			yesterday.wDay = 31;
		}
		else if (today.wMonth == 5 || today.wMonth == 7 || today.wMonth == 10 || today.wMonth == 12)
		{
			yesterday.wDay = 30;
		}
		else if (today.wMonth == 3)
		{
			if ((today.wYear % 4 == 0) && (today.wYear % 100 != 0) || (today.wYear % 400 == 0))
				yesterday.wDay = 29;
			else
				yesterday.wDay = 28;
		}
		else if (today.wMonth == 1)
		{
			yesterday.wMonth = 12;
			yesterday.wYear -= 1;
		}
	}
}

void TFrmData::DealMatchDateTime(vector<TMatch*>vecMatch)
{
	SYSTEMTIME stTime = { 0 };
	GetLocalTime(&stTime);
	char szDate[20] = { 0 };
	SYSTEMTIME stDateY = { 0 };
	SYSTEMTIME stDateYY = { 0 };
	SYSTEMTIME stDateYYY = { 0 };
	GetYesterday(stTime ,stDateY);
	GetYesterday(stDateY, stDateYY);
	GetYesterday(stDateYY, stDateYYY);

	for (uint i = 0; i < vecMatch.size(); i++)
	{
		const TMatch* pMatch = vecMatch.at(i);
		if (strcmp(pMatch->ExchangeNo, exSHFE) == 0 || strcmp(pMatch->ExchangeNo, exCFFEX) == 0)
		{
			m_vecMatchDateTime.push_back(pMatch->MatchDateTime);
			continue;
		}
		SYSTEMTIME stMatchTime = {0};
		SplitDateTime(pMatch->MatchDateTime, stMatchTime);	
		strcpy_s(szDate, pMatch->MatchDateTime);
		switch (stTime.wDayOfWeek)
		{
		case 0:
			if (stMatchTime.wHour > 20)
				SetDate(szDate, stDateYY);
			break;
		case 1:
			if (stTime.wHour < 21)
			{
				if (stMatchTime.wHour > 20)
					SetDate(szDate, stDateYYY);
				else
					SetDate(szDate, stTime);
			}
			else if (stTime.wHour > 20)
			{
				if (stMatchTime.wHour > 20)
					SetDate(szDate, stTime);
			}
			break;
		case 2:
		case 3:
		case 4:
		case 5:
			if (stTime.wHour<21 &&stMatchTime.wHour > 20)  //减一天
			{
				SetDate(szDate, stTime);
			}
			break;
		case 6:
			if (stMatchTime.wHour>20)
				SetDate(szDate, stDateY);
			break;
		}
		m_vecMatchDateTime.push_back(szDate);
	}
}
void TFrmData::SetDate(char* date, SYSTEMTIME &stTime)
{
	date[2] = stTime.wYear % 100 / 10 + 0x30;
	date[3] = stTime.wYear % 10 + 0x30;
	date[4] = '-';
	date[5] = stTime.wMonth / 10 + 0x30;
	date[6] = stTime.wMonth % 10 + 0x30;
	date[7] = '-';
	date[8] = stTime.wDay / 10 + 0x30;
	date[9] = stTime.wDay % 10 + 0x30;
}
void TFrmData::SplitDateTime(const char* dateTime, SYSTEMTIME &t)
{
	string strTime = dateTime;
	string::size_type iSplit = strTime.find(' ');
	if (iSplit != string::npos)
	{
		string date = strTime.substr(0, iSplit);
		strTime = strTime.substr(++iSplit, string::npos);
		iSplit = date.find('-');
		string strTemp = date.substr(0, iSplit);
		date = date.substr(++iSplit, string::npos);
		t.wYear = atoi(strTemp.c_str());
		iSplit = date.find('-');
		strTemp = date.substr(0, iSplit);
		date = date.substr(++iSplit, string::npos);
		t.wMonth = atoi(strTemp.c_str());
		t.wDay = atoi(date.c_str());

		iSplit = strTime.find(':');
		strTemp = strTime.substr(0, iSplit);
		t.wHour = atoi(strTemp.c_str());
		strTime = strTime.substr(++iSplit, string::npos);
		iSplit = strTime.find(':');
		strTemp = strTime.substr(0, iSplit);
		t.wMinute = atoi(strTemp.c_str());
		strTime = strTime.substr(++iSplit, string::npos);
		t.wSecond = atoi(strTime.c_str());
	}
}