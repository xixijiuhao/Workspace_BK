#pragma once
class ACAnalysisPresenter;
class KLineView : public TIxFrm
{
public:
	KLineView();
	~KLineView();
public:
	void CreateView(HWND parent, CRECT& rect);
	void MoveWindow(CRECT& rect);
	void SetPresenter(ACAnalysisPresenter* presenter) { m_clPresenter = presenter; }
	void SetKLineModel(KLineModel* model) {
		m_clModel = model; 	
	}
	void UpdateKLineView(ACContract &contract);

	POINT GetPointByPriceAndDate(int nDate, SPriceType price);

	KLineChartRect& GetKLineChartRect() { return m_clRect; }
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnCreate();
	void OnPaint();
	void OnSize();
	void InitChartRect(CRECT rect);
	void OnDealSetFocus();
public:
	//�μ�����
	void DrawTime(TMemDC& dc, RECT r, int index);
	void DrawAxisPrice(TMemDC& dc, RECT r, SPriceType price);
	void DrawACImage(TMemDC &dc);
	void DrawKlineAndAC(TMemDC &dc);

public:
	ACContract				m_KContract;
private:
	KLineModel*     m_clModel;
	ACAnalysisPresenter* m_clPresenter;
	KLineChartRect	m_clRect;
private:
	HWND			m_iParentHwnd;
	TCHAR  m_szImgFilePath[200];
	Gdiplus::Image*  m_Img;
	Gdiplus::Rect m_frmRect;
	ULONG_PTR gdiplusStartupToken;

//�ƶ�K��
private:
	BOOL			m_bMouseTract;
	POINT			m_BeginPoint;
	BOOL			m_bMouseDown;
	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnMouseLeave();
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnLButtonUp(WPARAM wParam, LPARAM lParam);
	void TraceMouse();
	void ReleaseMouse();
private:
	//�������
	void DrawFrame(TMemDC& dc);
	//��Xʱ����
	void DrawBottomTime(TMemDC& dc);
	//��Y������
	void DrawYAxisData(TMemDC& dc);
	//����Լ����
	void DrawContractName(TMemDC& dc);
	//��K��
	void DrawKLine(TMemDC& dc);
	//�����¼�
	void DrawLastPriceLine(TMemDC& dc);
	//��ѹ��λ������λ
	void DrawTCRSLine(TMemDC& dc);
	//����ʱ����������Ϣ(  ��Ƶ�͵��� ���չʾ���ƣ�Ŀǰ����ʾST )
	void DrawTrend(TMemDC& dc);
	void DrawFibonacciTrend(TMemDC& dc);
	void DrawBigMovement(TMemDC& dc);
	void DrawConsecutiveCandles(TMemDC& dc);

	void GetPointVector(int nDate, SPriceType price, std::vector<POINT> &pointVec);
	void GetPosYVector(SPriceType price, std::map<int, std::string> &posMap);

	void GetImgeWithPath(const wchar_t* imageName);
	void DrawResSupLine(TMemDC& dc, SPriceType price, std::string name);
private:
	//��ʮ�ֹ��
	BOOL			m_bShowCross;
	void DrawCross(TMemDC& dc);
};

