#ifndef _T_TRADE_REPORT_H
#define _T_TRADE_REPORT_H

enum TBoxState
{
	boxStateNormal = 0,
	boxStateHover,
	boxStateClicked
};

class TTradeReport:public TIxFrm
{
public:
	TTradeReport(); 
	~TTradeReport();
	void ShowFrm();
	static void OnHotKeyDown(const bool Alt, const bool Ctrl, const bool Shift, const unsigned char VKey, const bool Exclusive, HWND source);
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:
	void OnCreate();
	void OnPaint();
	void OnComboxSelect(WPARAM wParam, LPARAM lParam);
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnLButtonUp(WPARAM wParam, LPARAM lParam);
	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnMouseLeave(WPARAM wParam, LPARAM lParam);
	void OnClickBtn(WPARAM wParam, LPARAM lParam);
	void DrawText(TMemDC *pMemDC);
	void DrawFrame(TMemDC *pMemDC);
	void DrawClose(TMemDC *pMemDC);
	LRESULT OnCtrlStatic(WPARAM wParam, LPARAM lParam);
	void OnDrawLV(WPARAM wParam, LPARAM lParam);
	void OnMeasureLV(WPARAM wParam, LPARAM lParam);
	void OnCbSelected(WPARAM wParam, LPARAM lParam);
private:
	void SetLabelText();
	void InitLV();
	void SetLVData();
	void FormatTime(char *pTimeStr, int length, double dSecond);

	void SetSize();
	void DrawMaxMinBox(TMemDC *pMemDC);
	void MoveWindow();

	void Redraw();

	bool IsAuthent();
	bool ReadVersion();
private:
	TCombox         m_cbTime;       // ѡ�����ɱ����ʱ�䷶Χ
	TStaticButton   m_btnReport;    // ���ɱ��水ť
	TListView       m_lvData;       // �����б�
	TStatic         m_labelTime;    // ��ǩ
	TStatic         m_labelMoney[4];   //�ڳ��˻���ֵ������ӯ������ĩ�˻���ֵ
private:
	vector<long long>*  m_pVecTradeData;// ��ͼҪ�õ�������
	int                m_iTimeRange;
	HBRUSH             m_hbrStatic;
	double             m_dProfit;              //����ӯ��
	 //ƽ��ֵ���������
	double             m_dFee;
	vector<string>*    m_pVecContractNo;
	RECT               m_rcClose;
	RECT               m_rcMaxMinBox;
	bool               m_bIsFullScreen;
	bool               m_bTrackMouse;
	TBoxState          m_boxState[2];
private:
	TChartFrm         m_frmChart;
	TFrmData          m_frmData;

private:
	enum{periodOne,periodTwo,periodThree,periodFour,periodFive,periodSix,daytime,night,allday};
};


#endif