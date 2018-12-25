#ifndef _T_VIEW_THREE_KEY_H_
#define _T_VIEW_THREE_KEY_H_

enum
{
	ID_frmTKeyOrder = 0,
	ID_frmOptionConfig,
	ID_frmDefaultQty,
	ID_frmSuperPrice,
	ID_frmChasePrice,
	ID_frmStopLoss,
};

class TViewThreeKey :public TIxFrm,public IColleague
{
public:
	TViewThreeKey(IMediator* mediator);
	~TViewThreeKey();
	void ShowFrm();
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	//发送消息
	void Send(unsigned int CommandID, const char* content);
	RECT GetShowRect();
public:
	//设置初始窗口位置
	void SetToOriginalPos();
public:
	void ShowHide(int iShow = 0);
protected:
	void OnPaint();
	void OnCreate();
	void OnDestroy();
	void OnNcDestroy();
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnMouseLeave();
	void OnSize(WPARAM wParam, LPARAM lParam);
	void OnNotify(WPARAM wParam, LPARAM lParam);
	void ShowHideTree(LPWSTR lpsztext);
    void OnComboxSelect(WPARAM wParam, LPARAM lParam);

	//实现鼠标改变窗口大小
	UINT OnNcHitTest(WPARAM wParam, LPARAM lParam);
	void OnSetCursor(WPARAM wParam, LPARAM lParam);
	void OnNcLButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnCtlColor(WPARAM wParam, LPARAM lParam);
	void OnSizing(WPARAM wParam, LPARAM lParam);

	void DraeLeftArrow(HDC hdc,int left, int top);
	void DrawClose(HDC hdc); 
public:
	void OnUserLogin(int index);
	void UpdateMoney(const TMoney* pMoney);
	void UpdateMoney();
private:
	TreeView      m_treeSetting;
	TStatic       m_sFundInfo;
	//CxCombox      m_cbUserNo;
    TComboxList      m_cblistUserNo;

private:
	char m_strTitle[128];
private:
	//const变量这样命名
	const int caption_height = 30;
	const int frame_height = 389;
	const int frame_width = 1166;//
	HBRUSH   m_hbrCaption;
	COLORREF caption_focus_color;
	bool     m_bMouseTrack;

	int		m_iSelTree;
	RECT    m_PosRect;
	RECT    m_rcUserNo;
	int		m_iCbSelIndex;
	vector<string>* m_pVecComboxCont;
	map<string, vector<string>>* m_pMapCurrencyNo;
	bool	m_bUserChange;
	RECT		   m_rcClose;
	bool		 m_bHoverClose;
private:
	int      m_iOffset; //设置（tree）隐藏的话，界面要往左移动，默认有设置
	int      m_iFrmTop;
public:
	enum
	{
		CMD_TView_SwitchFrm,
		CMD_TView_SizeChange,
		CMD_TView_UserChange,

		CMD_TView_GetComboxCont,
		CMD_TView_GetCurrencyNo,
	};
};
#endif