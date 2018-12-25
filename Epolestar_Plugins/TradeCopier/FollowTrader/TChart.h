#ifndef _T_CHART_H_
#define _T_CHART_H_

class TChartFrm :public TIxFrm
{
public:
	TChartFrm();
	~TChartFrm();
	void Create(HWND hParent);
	void SetData(vector<long long>*pMoneyData);
	void MoveWindow(int x, int y, int cx, int cy);
	void RegistData(vector<long long>*pMoneyData){ m_pTradeData = pMoneyData; }
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnCreate();
	void OnPaint();
	void DrawArrow(TMemDC *pMemDC, RECT rect);
	void DrawScale(TMemDC *pMemDC);
	void DrawLine(TMemDC *pMemDC);
private:
	void DealData();
	long long GetMaxDiff();
	void CalcData();
	void SetPoint(long long maxDiff);
	void CalcYLineGap(long long maxDiff);
	void SetChartSize();
	void InitData();
private:
	vector<long long>* m_pTradeData;// ��ͼҪ�õ�������
	vector<POINT>      m_vPoints;
	int                m_iXLength;
	int				   m_iYHeight;
	RECT			   m_rcChart;
	double             m_dVerticalGap;          //����Ǯ����һ�����ص�
	int                m_iHorziontalGap;        //ˮƽ���
	POINT              m_startPt;
	bool               m_bIsLine;               //����Ƿ�Ϊֱ��
	int                m_iIndexGap;
	int                m_iDrawIndxGap;
	int                m_iDrawThresh;

	int                m_iYVerticalGap;
	int                m_dYPerMoney;
	vector<int>        m_vecYLine;
	vector<wstring>    m_vecYLineString;
	int                m_iVTextWidth;
};

#endif