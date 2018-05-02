#ifndef _T_FRM_DATA_H
#define _T_FRM_DATA_H

class TFrmData
{
public:
	TFrmData();
	~TFrmData();
	void GetData();
	double GetFee(){ return m_dFee; }
	double GetProfit(){ return m_dProfit; }
	void SetTimeRange(int timeRange){ m_iTimeRange = timeRange; }
	vector<long long >*GetTradeData(){ return &m_vecTradeData; }
	vector<string>*  GetCbString(){ return &m_vecContractNo; }
private:
	bool DealData();
	void Sort(TDataContainer &data);
	bool SetRangeTime(char *strBeginTime, char *strEndTime, int length = 21);
	bool SetRangeTime(int iBegin, int iEnd);
	bool DealTimeRange(vector<TMatch*> &matchs, double &dMoney); //返回开始时间的当前可用money
	void GetYesterday(SYSTEMTIME &today, SYSTEMTIME &yesterday);
	double GetPositionSubMoney(TMatch *pMatch);
	bool GetMatch(TDataContainer &matchs);
	void GetContractNo(vector<TMatch*> &vMatchs);
	bool DealFriday(char *strBeginTime, char *strEndTime, int length =21);

	void DealMatchDateTime(vector<TMatch*> vecMatch);
	void TFrmData::SplitDateTime(const char* dateTime, SYSTEMTIME &stTime);
	void SetDate(char* date, SYSTEMTIME &stTime);
private:
	double             m_dFee;
	char               m_szUserNo[21];
	vector<string>     m_vecContractNo;
	double             m_dProfit;              //交易盈亏
	int                m_iTimeRange;
	map<string, char>  m_mapPositionSub;
	vector<long long > m_vecTradeData;
	vector<string>	   m_vecMatchDateTime;
private:
	enum{ periodOne, periodTwo, periodThree, periodFour, periodFive, periodSix, daytime, night, allday };
};

#endif