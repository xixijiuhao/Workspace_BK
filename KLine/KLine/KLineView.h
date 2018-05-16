#pragma once
class KLinePresenter;
class KLineView : public TIxFrm
{
public:
	KLineView();
	~KLineView();
public:
	void CreateView(HWND parent, CRECT& rect);
	void MoveWindow(CRECT& rect);
	void SetPresenter(KLinePresenter* presenter) { m_clPresenter = presenter; }
	void SetKLineModel(KLineModel* model) {
		m_clModel = model; 	
	}
	void UpdateKLineView();

	KLineChartRect& GetKLineChartRect() { return m_clRect; }
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnCreate();
	void OnPaint();
	void OnSize();
	void InitChartRect(CRECT rect);
public:
	//次级函数
	void DrawTime(TMemDC& dc, RECT r, int index);
	void DrawAxisPrice(TMemDC& dc, RECT r, SPriceType price);
private:
	KLineModel*     m_clModel;
	KLinePresenter* m_clPresenter;
	KLineChartRect	m_clRect;
private:
	HWND			m_iParentHwnd;
	TCHAR  m_szImgFilePath[200];
	Gdiplus::Image*  m_Img;
	Gdiplus::Rect m_frmRect;
	ULONG_PTR gdiplusStartupToken;

//移动K线
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
	//画框架线
	void DrawFrame(TMemDC& dc);
	//画X时间轴
	void DrawBottomTime(TMemDC& dc);
	//画Y坐标轴
	void DrawYAxisData(TMemDC& dc);
	//画合约名称
	void DrawContractName(TMemDC& dc);
	//画K线
	void DrawKLine(TMemDC& dc);
	//画最新价
	void DrawLastPriceLine(TMemDC& dc);
	//画压力位和阻力位
	void DrawTCRSLine(TMemDC& dc);
	//更新时间点和趋势信息(  高频和当日 如何展示趋势，目前都显示ST )
	void DrawTrend(TMemDC& dc);
	void GetImgeWithPath(const wchar_t* imageName);
	void DrawResSupLine(TMemDC& dc, SPriceType price, std::string name);
private:
	//画十字光标
	BOOL			m_bShowCross;
	void DrawCross(TMemDC& dc);
};


