#include "BaseInclude.h"
int KLineWidth = 10;
int KLineVap = 1;
KLineSlice slice = KLine_Min30;
SKLineTypeType type = S_KLINE_MINUTE;
int KLineUtil::GetKLineVap()
{
	return KLineVap;
}
int KLineUtil::GetKLineTotalWidth()
{
	return KLineWidth + KLineVap;
}
int KLineUtil::GetKLineWidth()
{
	return KLineWidth;
}
void KLineUtil::SetKLineWidth(int width)
{
	KLineWidth = width;
}
void KLineUtil::SetKLineVap(int vap)
{
	KLineVap = vap;
}
KLineSlice KLineUtil::GetKLineSlice()
{
	return slice;
}
void KLineUtil::SetKLineSlice(KLineSlice iSlice)
{
	slice = iSlice;
}
SKLineTypeType KLineUtil::GetKLineType()
{
	return type;
}
void KLineUtil::SetKLineType(SKLineTypeType iType)
{
	if (iType == S_KLINE_MINUTE || iType == S_KLINE_DAY) {
		type = iType;
	}
}
std::string KLineUtil::GetDateTime(SDateTimeType time)
{
	string strDateTime;
	return strDateTime;
}
RECT KLineUtil::CRectToRect(const CRECT &cr)
{
	RECT r;
	r.left = cr.x;
	r.right = cr.x + cr.cx;
	r.top = cr.y;
	r.bottom = cr.y + cr.cy;
	return r;
}
void KLineUtil::GetDrawTime(SHisQuoteData d, wchar_t* wtext, int size)
{
	SDateType date = (SDateType)(d.DateTimeStamp / 1000000000LL);
	STimeType time = (STimeType)(d.DateTimeStamp / 1000LL % 1000000LL);
	swprintf_s(wtext, size, L"%04d/%02d/%02d %02d:%02d:%02d", date / 10000, date / 100 % 100, date % 100,
		time / 10000, time / 100 % 100, time % 100);
}
void KLineUtil::TCTimeToQuoteTime(char* tcDate, size_t dateSize, char* tcHour, size_t hourSize, SDateTimeType& dateTime)
{
	SDateTimeType date = atoi(tcDate);
	for (int i = 0; i < hourSize; i++) {
		if (tcHour[i] == ':' && i + 1 != hourSize) {
			for (int j = i; j < hourSize; j++) {
				if (tcHour[j] != '\0') {
					tcHour[j] = tcHour[j + 1];
				}
				else {
					break;
				}
			}
			break;
		}
	}
	SDateTimeType _time = atoi(tcHour);

	int iYear = date / 10000;
	int iMonth = date / 100 % 100;
	int iDate = date % 100;
	int iHour = _time / 100;
	int iMinute = _time % 100;

	tm then_tm;
	then_tm.tm_year = iYear - 1900;
	then_tm.tm_mon = iMonth - 1;
	then_tm.tm_mday = iDate;
	then_tm.tm_hour = iHour;
	then_tm.tm_min = iMinute;
	then_tm.tm_sec = 0;

	time_t timetThen = mktime(&then_tm);
	chrono::time_point<chrono::system_clock> then_tp = chrono::system_clock::from_time_t(timetThen);
	auto nowDate = then_tp + chrono::hours(8);
	time_t now_c = chrono::system_clock::to_time_t(nowDate);
	tm* now_tm = localtime(&now_c);
	iYear = now_tm->tm_year + 1900;
	iMonth = now_tm->tm_mon + 1;
	iDate = now_tm->tm_mday;
	iHour = now_tm->tm_hour;
	iMinute = now_tm->tm_min;

	char sDate[50] = { 0 };
	sprintf_s(sDate, "%4d%02d%02d%02d%02d%02d%03d", iYear, iMonth, iDate, iHour, iMinute, 0, 0);
	sprintf_s(tcDate, dateSize, "%4d%02d%02d", iYear, iMonth, iDate);
	sprintf_s(tcHour, hourSize, "%2d:%02d", iHour, iMinute);
	dateTime = atoll(sDate);
}
void KLineUtil::GetTCTerm(int index, char* term, int size)
{
	switch (index)
	{
	case 0:
		strcpy_s(term, size, TC_15Min);
		break;
	case 1:
		strcpy_s(term, size, TC_intraday);
		break;
	case 2:
		strcpy_s(term, size, TC_shortTerm);
		break;
	case 3:
		strcpy_s(term, size, TC_shortTerm);
		break;
	default:
		break;
	}
}
void KLineUtil::DrawTrend(TMemDC& dc, KLinePoint point, TCOPinion trend, bool drawArraw)
{
	KLineSaveDC Save(dc);
	HPEN pen = CreatePen(PS_SOLID, 3, KLineFontNameColor);
	SelectObject(dc.GetHdc(), pen);
	int slice = 3 * KLineUtil::GetKLineTotalWidth();


	//画直线
	POINT ptFrom = { point.x + KLineUtil::GetKLineWidth() / 2,  (point.CloseY > point.OpenY ? point.CloseY : point.OpenY) };
	POINT ptTo = ptFrom;
	if (drawArraw) {
		MoveToEx(dc.GetHdc(), ptFrom.x, ptFrom.y, NULL);
		switch (trend)
		{
		case TC_opinion_up:
			ptTo.y -= slice;
			break;
		case TC_opinion_limitUp:
			ptTo.x += slice;
			ptTo.y -= slice;
			break;
		case TC_opinion_wave:
			ptTo.x += slice;
			break;
		case TC_opinion_limitDown:
			ptTo.x += slice;
			ptTo.y += slice;
			break;
		case TC_opinion_down:
			ptTo.y += slice;
			break;
		default:
			break;
		}
		LineTo(dc.GetHdc(), ptTo.x, ptTo.y);
		double slopy = 0, cosy = 0, siny = 0;
		double par = 8.0; //length of Arrow (>)
		slopy = atan2(double(ptFrom.y - ptTo.y), double(ptFrom.x - ptTo.x));
		cosy = cos(slopy);
		siny = sin(slopy);
		//画箭头
		POINT pt1, pt2;
		pt1.x = ptTo.x + int(par*cosy - (par / 2.0*siny));
		pt1.y = ptTo.y + int(par*siny + (par / 2.0*cosy));
		pt2.x = ptTo.x + int(par*cosy + par / 2.0*siny);
		pt2.y = ptTo.y - int(par / 2.0*cosy - par*siny);
		MoveToEx(dc.GetHdc(), pt1.x, pt1.y, 0);
		LineTo(dc.GetHdc(), ptTo.x, ptTo.y);
		MoveToEx(dc.GetHdc(), pt2.x, pt2.y, 0);
		LineTo(dc.GetHdc(), ptTo.x, ptTo.y);
	}
	//更新原点
	int half = KLineUtil::GetKLineWidth() / 2;
	Ellipse(dc.GetHdc(), ptFrom.x - half, ptFrom.y - half, ptFrom.x + half, ptFrom.y + half);

	DeleteObject(pen);
}

void KLineUtil::NewDrawTrend(TMemDC& dc, KLinePoint point,POINT to,  bool up)
{
	KLineSaveDC Save(dc);
	HPEN redPen = CreatePen(PS_SOLID, 2, KLineRedColor);
	HPEN greenPen = CreatePen(PS_SOLID, 2, KLineGreenColor);
	if (up)
		SelectObject(dc.GetHdc(), redPen);
	else
		SelectObject(dc.GetHdc(), greenPen);
	int sliceX = to.x - point.x;


	//画直线
	int ArrawVap = 10;

	//int fromY = up ? point.HighY : point.LowY;
	//POINT ptFrom = { point.x + KLineUtil::GetKLineWidth() / 2, fromY };

	int fromY = (point.CloseY + point.OpenY) / 2;
	POINT ptFrom = { point.x + KLineUtil::GetKLineWidth() * 2, fromY };
	POINT ptTo = ptFrom;
	MoveToEx(dc.GetHdc(), ptFrom.x, ptFrom.y, NULL);
	ptTo.x = to.x - ArrawVap;
	if(up){
		ptTo.y = to.y + ArrawVap;
	}
	else {
		ptTo.y = to.y - ArrawVap;
	}

	LineTo(dc.GetHdc(), ptTo.x, ptTo.y);

	double slopy = 0, cosy = 0, siny = 0;
	double par = 8.0; //length of Arrow (>)
	slopy = atan2(double(ptFrom.y - ptTo.y), double(ptFrom.x - ptTo.x));
	cosy = cos(slopy);
	siny = sin(slopy);
	//画箭头
	POINT pt1, pt2;
	pt1.x = ptTo.x + int(par*cosy - (par / 2.0*siny));
	pt1.y = ptTo.y + int(par*siny + (par / 2.0*cosy));
	pt2.x = ptTo.x + int(par*cosy + par / 2.0*siny);
	pt2.y = ptTo.y - int(par / 2.0*cosy - par*siny);
	MoveToEx(dc.GetHdc(), pt1.x, pt1.y, 0);
	LineTo(dc.GetHdc(), ptTo.x, ptTo.y);
	MoveToEx(dc.GetHdc(), pt2.x, pt2.y, 0);
	LineTo(dc.GetHdc(), ptTo.x, ptTo.y);
	//更新原点
	int half = KLineUtil::GetKLineWidth() / 2;
	Ellipse(dc.GetHdc(), ptFrom.x - half, ptFrom.y - half, ptFrom.x + half, ptFrom.y + half);

	DeleteObject(redPen);
	DeleteObject(greenPen);
}

void KLineUtil::NewDrawTrendWithImage(KLinePoint point, POINT to, bool up, POINT &imagePoint)
{
	int sliceX = to.x - point.x;
	int ArrawVap = 10;
	int fromY = (point.CloseY + point.OpenY) / 2;
	imagePoint = { point.x + KLineUtil::GetKLineWidth() * 2, fromY };
	imagePoint.x = to.x - ArrawVap;

	//int sliceX = to.x - point.x;
	//int ArrawVap = 10;

	//int fromY = (point.CloseY + point.OpenY) / 2;
	//POINT ptFrom = { point.x + KLineUtil::GetKLineWidth() * 2, fromY };
	//POINT ptTo = ptFrom;
	//MoveToEx(dc.GetHdc(), ptFrom.x, ptFrom.y, NULL);
	//ptTo.x = to.x - ArrawVap;
	//if (up) {
	//	ptTo.y = to.y + ArrawVap;
	//}
	//else {
	//	ptTo.y = to.y - ArrawVap;
	//}

	//LineTo(dc.GetHdc(), ptTo.x, ptTo.y);

	/*imagePoint.x = to.x - ArrawVap;*/
	/*if (up)
	{
		imagePoint.y = to.y + ArrawVap;
	}
	else
	{
		imagePoint.y = to.y - ArrawVap;
	}*/
}
