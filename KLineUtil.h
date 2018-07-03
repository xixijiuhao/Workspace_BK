#pragma once
namespace KLineUtil
{
	//K�߿��
	int GetKLineTotalWidth();
	int GetKLineWidth();
	void SetKLineWidth(int width);
	int GetKLineVap();
	void SetKLineVap(int vap);
	//����
	KLineSlice GetKLineSlice();
	void SetKLineSlice(KLineSlice iSlice);
	//���� ������
	SKLineTypeType GetKLineType();
	void SetKLineType(SKLineTypeType iType);
	//ʱ��ת��
	std::string GetDateTime(SDateTimeType time);
	void GetDrawTime(SHisQuoteData d, wchar_t* wtext, int size);
	void TCTimeToQuoteTime(char* tcDate, size_t dateSize, char* tcHour, size_t hourSize, SDateTimeType& dateTime);
	void GetTCTerm(int index, char* term, int size);
	//����ת��
	RECT CRectToRect(const CRECT &cr);
	//��ͼ��
	void DrawTrend(TMemDC& dc,KLinePoint point, TCOPinion trend, bool drawArraw = true);
	void NewDrawTrend(TMemDC& dc, KLinePoint from, POINT to, bool up);
	void NewDrawTrendWithImage(KLinePoint point, POINT to, bool up, POINT &imagePoint);
};


