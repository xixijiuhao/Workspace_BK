#pragma once

#define MainFrame_Height 800
#define MainFrame_Width 1200

#define KLineChart_Height 600
#define KLineChart_Width 1000

#define KLineIntroduct_Height  300
#define KLineIntroduct_Width   1080
#define KListView_Height 575
#define KListView_Width 200


#define KLineView_TopHeight 40
#define KLineView_LeftWidth 50
#define KLineView_RightWidth 30
#define KLineView_BottomHeight 25

//#define KLineIntroduct_Height  (MainFrame_Height - KLineChart_Height - CAPTION_HEIGHT)
//#define KLineIntroduct_Width   (MainFrame_Width - KLineView_LeftWidth - KLineView_RightWidth)
//#define KListView_Height (MainFrame_Height - CAPTION_HEIGHT - KLineIntroduct_Height - KLineView_TopHeight)
//#define KListView_Width (MainFrame_Width - KLineChart_Width)



#define KLineView_TextVap 36

#define KLineUpColor RGB(255,60,57)
#define KLineDownColor RGB(0,244,244)
#define KLineFrameColor RGB(46,46,46)
#define KLineFontNumberColor RGB(171,180,187)
#define KLineFontNameColor RGB(200,200,200)
#define KLineRedColor RGB(255, 60, 56)
#define KLineGreenColor RGB(11, 163, 77)
#define KlineBlueColor RGB(13, 187, 239)


const int  KLineMaxCount = 1200;//Ԥ������1200������
struct KLinePoint
{
	int x;
	int HighY;
	int OpenY;
	int CloseY;
	int LowY;
};
struct CRECT
{
	int x;
	int y;
	int cx;
	int cy;
};
class MainFrmRect
{
public:
	CRECT TopRect;
	CRECT KLineChartRect;
	CRECT IntroductRect;
	CRECT CommodityListRect;
};
class KLineChartRect
{
public:
	RECT TopRect;//��ʾ����
	RECT LeftRect;//���Y��
	RECT RightRect;//�Ҳ�Y��
	RECT BottomRect;//X�ᣬ��ʾʱ��
	RECT CenterRect;//KLineChart
	RECT CenterTopRect;//��ʾָ����Ϣ
	RECT CenterBottomRect;//��ʾ����KLine��Ϣ
	RECT CenterKLineRect;//���ڻ�KLine����
};

class KLineSaveDC
{
public:
	KLineSaveDC(TMemDC& dc):m_dc(dc)
	{ 
		m_iSave = SaveDC(dc.GetHdc());
	}
	~KLineSaveDC()
	{
		RestoreDC(m_dc.GetHdc(), m_iSave);
	}
private:
	TMemDC&	m_dc;
	int		m_iSave;
};

typedef std::vector<SHisQuoteData> vtDrawData;
typedef std::vector<KLinePoint>   vtDrawPoint;


