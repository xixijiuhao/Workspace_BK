#pragma once
namespace KLineUtil
{
	//K线宽度
	int GetKLineTotalWidth();
	int GetKLineWidth();
	void SetKLineWidth(int width);
	int GetKLineVap();
	void SetKLineVap(int vap);
	//周期
	KLineSlice GetKLineSlice();
	void SetKLineSlice(KLineSlice iSlice);
	//日线 分钟线
	SKLineTypeType GetKLineType();
	void SetKLineType(SKLineTypeType iType);
	//时间转化
	std::string GetDateTime(SDateTimeType time);
	void GetDrawTime(SHisQuoteData d, wchar_t* wtext, int size);
	void TCTimeToQuoteTime(char* tcDate, size_t dateSize, char* tcHour, size_t hourSize, SDateTimeType& dateTime);
	void GetTCTerm(int index, char* term, int size);
	//矩形转化
	RECT CRectToRect(const CRECT &cr);
	//画图形
	void DrawTrend(TMemDC& dc,KLinePoint point, TCOPinion trend, bool drawArraw = true);
	void NewDrawTrend(TMemDC& dc, KLinePoint from, POINT to, bool up);
};


