#pragma once

class TradeModel
{
public:
	TradeModel();
public:
	bool CanTrade(const TPosition * pPosition);
	void IncrementTime();
	void SetTime();
private:
	void Init();
	void SetCFFEXTradeTime();
	void SetNightTradeTime();

	bool CheckCanTrade(TradeTime & stTime);

	bool CheckDay(const TPosition* pPos);
	bool CheckNight(const TPosition* pPos);
private:
	map<string, TradeTime> m_dayTradeTime;
	map<string, TNightTradeTime> m_nightTradeTime;
	int                    m_iNowTime;
};
