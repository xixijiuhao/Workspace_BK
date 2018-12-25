#include "PreInclude.h"


#define MorningBreakBegin  10
#define MorningBreakEnd    

TradeModel::TradeModel()
{
	Init();
	SetTime();
}

bool TradeModel::CanTrade(const TPosition * pPosition)
{
	if (!pPosition) return false;

	if (m_iNowTime > 3 * 3600 && m_iNowTime < 20 * 3600)
	{
		return CheckDay(pPosition);
	}
	else
	{
		return CheckNight(pPosition);
	}

	

}

bool TradeModel::CheckNight(const TPosition* pPosition)
{
	char szKey[50];
	sprintf_s(szKey,"%s %s", pPosition->ExchangeNo, pPosition->CommodityNo);

	auto iter = m_nightTradeTime.find(szKey);

	if (iter != m_nightTradeTime.end())
	{
		if (m_iNowTime >= iter->second.BeginTime && m_iNowTime <= iter->second.EndTime)
			return true;
	}

	return false;
}

bool TradeModel::CheckDay(const TPosition* pPosition)
{
	char szKey[50] = { 0 };
	strcpy_s(szKey, pPosition->ExchangeNo);

	if (!strcmp(szKey, exCFFEX))
	{
		if (!strcmp(pPosition->CommodityNo, "TF") || !strcmp(pPosition->CommodityNo, "T"))
		{
			sprintf_s(szKey, "%s %s", pPosition->ExchangeNo, pPosition->CommodityNo);
		}
	}
	auto iter = m_dayTradeTime.find(szKey);

	if (iter != m_dayTradeTime.end())
	{
		return CheckCanTrade(iter->second);
	}

	return false;
}

bool TradeModel::CheckCanTrade(TradeTime & stTime)
{
	bool bCanTrade = false;
	for (int i = 0; i < ARRAYSIZE(stTime.BeginTime); i++)
	{
		if (m_iNowTime >= stTime.BeginTime[i] && m_iNowTime <= stTime.EndTime[i])
		{
			bCanTrade = true;
			break;
		}
	}
	return bCanTrade;
}

void TradeModel::Init()
{
	TradeTime stTradeTime;

	stTradeTime.BeginTime[0] = 9 * 3600;   //9：00
	stTradeTime.EndTime[0] = 10.25 * 3600; //10：15

	stTradeTime.BeginTime[1] = 10.5 * 3600; //10:30
	stTradeTime.EndTime[1] = 11.5 * 3600;   //11:30

	stTradeTime.BeginTime[2] = 13.5 * 3600; //13：30
	stTradeTime.EndTime[2] = 15 * 3600;    //15：00



//	char szText[100] = { 0 };
	//大商所 郑商所 交易时间一致
	m_dayTradeTime.insert(make_pair(exZCE,stTradeTime)); 
	m_dayTradeTime.insert(make_pair(exDCE, stTradeTime));
	m_dayTradeTime.insert(make_pair(exSHFE, stTradeTime));


	SetCFFEXTradeTime();

	SetNightTradeTime();
}



//中金所除了国债，其余合约交易时间均一致，所以其Key为CFFEX
//五债 十债 TF T
void TradeModel::SetCFFEXTradeTime()
{
	TradeTime stTradeTime;

	stTradeTime.BeginTime[0] = 9.5  * 3600;
	stTradeTime.EndTime[0] = 11.5 * 3600;

	stTradeTime.BeginTime[1] = 13 * 3600;
	stTradeTime.EndTime[1] = 15 * 3600;

	m_dayTradeTime.insert(make_pair(exCFFEX, stTradeTime));

	stTradeTime.BeginTime[0] = 9.25 * 3600;
	stTradeTime.EndTime[0] = 11.5 * 3600;

	stTradeTime.BeginTime[1] = 13 * 3600;
	stTradeTime.EndTime[1] = 15.25 * 3600;

	m_dayTradeTime.insert(make_pair("CFFEX TF", stTradeTime));
	m_dayTradeTime.insert(make_pair("CFFEX T", stTradeTime));
}

void TradeModel::SetTime()
{
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);

	m_iNowTime = stTime.wHour * 3600 + stTime.wMinute * 60 + stTime.wSecond;

	if (stTime.wHour < 3)
	{
		m_iNowTime += 86400;
	}
}

void TradeModel::IncrementTime()
{
	m_iNowTime++;

	if (m_iNowTime >= 27 * 3600)
	{
		SetTime();
	}
}

void TradeModel::SetNightTradeTime()
{
	char szFilePath[200] = { 0 };

	GetModulePath(szFilePath, sizeof(szFilePath), NightTradeTimeFilePath);

	char szAppName[][10] = {exZCE,exDCE,exSHFE};

	char szKeyName[10] = "0";
	int i = 0;
	int j = 0;
	char szContent[100] = { 0 };

	while (i<ARRAYSIZE(szAppName))
	{
		while (GetPrivateProfileStringA(szAppName[i], szKeyName, "", szContent, sizeof(szContent), szFilePath))
		{
			vector <string> result = CConvert::SplitString(szContent, ',');
			
			if (result.size() != 2)continue;

			vector<string> time = CConvert::SplitString(result[1], ':');

			if (result.size() == 2)
			{
				int hour = atoi(time[0].c_str());
				int minute = atoi(time[1].c_str());

				int second = hour * 3600 + minute * 60;
				if (hour < 9)
					second += 24 * 3600;

				char mapKey[50] = { 0 };
				sprintf_s(mapKey, "%s %s", szAppName[i], result[0].c_str());

				TNightTradeTime nightTime;
				nightTime.BeginTime = 21 * 3600;
				nightTime.EndTime = second;

				m_nightTradeTime.insert(make_pair(mapKey, nightTime));
			}
			j++;
			sprintf_s(szKeyName, "%d", j);
		}
		j = 0;
		strcpy_s(szKeyName, "0");
		i++;
	}

	
}