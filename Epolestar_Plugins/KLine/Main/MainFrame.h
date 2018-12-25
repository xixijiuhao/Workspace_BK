#pragma once

#define EMainFrmName L"class TMainFrame"
class KLinePresenter;
class TMainFrm :public VirtualWnd
{
public:
	TMainFrm();
	~TMainFrm();
	void CreateView();
	void setPresenter(KLinePresenter* presenter);

	void ChangeFrameRect(int width, int height);
private:
	LRESULT OnDealMsg(UINT message, WPARAM wParam, LPARAM lParam);
private:
	LRESULT OnNcDestory(WPARAM wParam, LPARAM lParam);
private:
	void OnPaint();
	void OnCreate();
	void OnSize(WPARAM wParam, LPARAM lParam);
private:
	void CreateSubView();
private:
	bool			m_bFirstShowSubView;
	MainFrmRect		m_MainFrmRect;
	KLinePresenter* m_Presenter;
	int  m_frmLeft;
};