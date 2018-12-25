#pragma once

enum KeyBoardSetControlID
{
	KB_START,

	KB_Set_GroupBox,
	KB_Normal_GroupBox,
	KB_Condition_GroupBox,
	KB_HK_GroupBox,

	KB_FocusPos_Check,
	KB_FocusPos_Edit,
	KB_FocusPos_Spin,
	KB_FocusPos_Static2,

	KB_UpDownEqualTab_Check,
	KB_RightLeftEqualTab_Check,

	KB_KeyBoardStyle_Static,
	KB_KeyBoardStyle_Combox,

	KB_KeyBoardFont_Static,
	KB_KeyBoardFont_Combox,
	KB_ShowOrderValidType_Check,

	KB_EnterSend_Check,

	KB_Buy_Static,
	KB_Buy_Edit,
	KB_Sell_Static,
	KB_Sell_Edit,
	KB_Open_Static,
	KB_Open_Edit,
	KB_CoverT_Static,
	KB_CoverT_Edit,
	KB_Cover_Static,
	KB_Cover_Edit,

	KB_Last_Static,
	KB_Last_Edit,
	KB_BuyPrice_Static,
	KB_BuyPrice_Edit,
	KB_SellPrice_Static,
	KB_SellPrice_Edit,
	KB_Greater_Static,
	KB_Greater_Edit,
	KB_Lesser_Static,
	KB_Lesser_Edit,

	KB_DefaultPrice_Static		,
	KB_DefaultPrice_Combox		,
	KB_DefaultPrice_Static2		,
	KB_DefaultPrice_Edit		,
	KB_DefaultPrice_Spin		,
	KB_DefaultPrice_Static3		,

	/*
	合约快捷键2017-5-25
	*/
	KB_HK_Contract_Hot_Static,
	KB_HK_Contract_Hot_Edit,
	KB_HK_Contract_Name_Static,
	KB_HK_Contract_Name_Edit,
	KB_HK_Contract_Name_Btn,
	KB_HK_Add_Btn,
	KB_HK_Del_Btn,
	KB_HK_List,
	//--
	/*
	追单助手2017-8-10
	*/
	KB_Start_Order_Assistant_Check,
	KB_AssistType_Combox,
	//
	KB_END
};

class KeyBoardSet:public TIxFrm, public IOperator
{
public:
	KeyBoardSet();
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	void Regist(IKeyOrderCfg* p);
	void UnRegist(IKeyOrderCfg* p);
private:
	set<IKeyOrderCfg*>		m_vNotify;										//参数变化通知对象容器

	void LayOut();															//重新布局
	RECT MoveCtrlWindow(HWND hwnd, int &nHeightStart, int &nHeightCenter);	//横向移动基准控件窗口
	RECT MoveCtrlRowWindow(HWND hwnd, int nHeightCenter);					//纵向移动随同变动控件窗口
	RECT MoveCtrlColWindow(HWND hwnd, int &nWidthMove);						//横向移动随同变动控件窗口
	void OnCreateWindow();													//窗口创建消息响应
	void OnNotify(WPARAM wParam, LPARAM lParam);
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnMouseWheel(WPARAM wParam, LPARAM lParam);
	void OnSpinDeltaUpDownNormal(int nDelta, int nID, int nMax, int nMin = 0);			//数量增减处理
private:
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);

	HWND				m_hControl[KB_END - KB_START + 1];					//控件句柄数组
	HBRUSH				m_hStaticBKBrush;									//静态文本背景画刷
private:
	void get_param_file(char * buf, size_t len);
	int read_param(const char * ident, const int def);
	void write_param(const char * ident, const int value);
	int read_paramHK(uint ident,  wstring& key,  wstring& value);
	void write_paramHK(uint ident, const wchar_t* value);

	bool LoadParam(bool bUpdateToUI = false);	
	bool SaveParam();

	void OnMeasureListView(LPARAM lParam);
	void OnDrawListView(WPARAM wParam, LPARAM lParam);


	void ChangeDefaultPrice();
	void ChangeFacadeFont(C_KeyBoardFontInfoCfg &fontcfg, bool bfromui = false,bool bNotify = false);
	void SetFontDefault(int nHeight, int nWidth, int nEscapement, int nOrientation, int fnWeight, DWORD fdwItalic, DWORD fdwUnderline, DWORD fdwStrikeOut, DWORD fdwCharSet, DWORD fdwOutputPrecision, DWORD fdwClipPrecision, DWORD fdwQuality, DWORD fdwPitchAndFamily, LPCTSTR lpszFace, LOGFONT &lf);
private:
	C_KeyBoardOrderCfg		m_KeyBoardCfg;
	C_KeyBoardFontInfoCfg	m_KyeBoardFontCfg;
private:
	CxCombox	m_ComboxFont;

	CxEdit		m_EditDefaultPos;
	CxEdit		m_Edit[10];

	CxSpin		m_SpinDefaultPos;

	CxCheck		m_CheckFocusPos;
	CxCheck		m_CheckUpDownEqualTab;
	CxCheck		m_CheckRightLeftEqualTab;
	CxCheck		m_CheckShowOrderValidType;
	CxCheck		m_CheckEnterSendOrder;

	CxCombox	m_ComboxStyle;

	
	CxCombox	m_ComboxDefPrice;	
	CxSpin		m_SpinDefPrice;
	CxEdit		m_EditDefPrice;

	/*
	合约快捷键2017-5-25
	*/
	CxEdit		m_EditCHotKey;
	CxEdit		m_EditCName;
	CxKeyBoardButton m_BtnContract;
	CPopWinCodeQ	m_PopupContractCode;
	CxKeyBoardButton m_BtnAdd;
	CxKeyBoardButton m_BtnDel;
	TListView	m_ListCHotKey;
	POINT m_ptPopup;
	std::map<std::wstring, std::wstring> m_stdmapContract;//合约快捷键配置
	//
	CxCheck		m_CheckStartOrderAssistant;
	CxCombox	m_ComboxAssistStyle;
private:
	void UpdateHKList();

public:
	virtual void SetPriceType(wstring sPType) {};
	virtual void SetValidType(wstring sVData) {};
	virtual void SetMaxMinQty(string sQty) {};
	virtual void SendOrder(MsgOrderInfo *msg) {};
	virtual void SetContractNo(string sCode, bool brepaint = true, bool bnutton = false, bool bgetfreeze = false);
	virtual void SetPriceData(string sdata) {};
	//--

public:
	map<int, WNDPROC> m_SetEdit;											//子类化窗口过程

	bool CheckValidKey(uint ID);
};

