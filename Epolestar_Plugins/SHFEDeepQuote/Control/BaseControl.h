/*******************************************************************************
* 文件名称:    BaseControl.h
* 创建日期:    
* 创 建 人:    
* 说    明:    
*******************************************************************************/
#pragma once

typedef struct CreateWndStruct
{
	int nWndType;					//窗体类型    
	DWORD dwstyle;					//窗体样式
	DWORD dwExStyle;				//窗口的扩展风格
	LPCTSTR lpszClass;				//指向注册类名的指针
	LPCTSTR lpszName;				//指向窗口名称的指针
	DWORD style;					//窗口风格
	HWND hwndParent;				//父窗口的句柄
	HMENU hMenu;					//菜单的句柄或是子窗口的标识符
	HINSTANCE hInstance;			//应用程序实例的句柄
	LPVOID lpCreateParams;          //指向窗口的创建数据
}CREATEWINDOW,CREATEWNDSTRUCT;

enum DATASTYLE{ 
	FLOATSTYLE = 1, 
	INTSTYLE = ES_NUMBER, 
};

typedef struct ControlPos
{
	int LEFT;
	int TOP;
	int RIGHT;
	int BOTTOM;
	int WIDGETH;
	int WIDGETV;
	int X_VAP;
	int Y_VAP;
	int ListViewV;
	int ListViewH;
	ControlPos()
	{
		LEFT = 10;
		TOP = 5;
		RIGHT = 130;
		BOTTOM = 20;
		WIDGETH = 20;
		WIDGETV = 120;
		X_VAP = 10;
		Y_VAP = 5;
		ListViewV = 440;
		ListViewH = 300;
	}
}CONTROL_POS;

/**=============================================================================
* 名    称:    <BaseControl>
* 说    明:     <控件的基类，从配置文件中读取控件属性，并提供控件的基本方法>
* 接    口:
* 调    用:
* 编    程:   
*=============================================================================*/
class BaseControl
{
private:
	static HWND CreateControl(CreateWndStruct &controlStruct);
public:
	BaseControl();
	virtual ~BaseControl();
public:
	/**------------------------------------------------------------------------------------
	* 名    称:  
	* 说    明: <设置控件位置>
	* 参    数:
	* 返    回:
	*---------------------------------------------------------------------------------------*/
	void OnMove();
	//void MoveWindow(int x, int y, int cx, int cy);
	void MoveWindow(int x, int y, int cx, int cy, UINT uFlags = SWP_NOZORDER);
	void MoveWindow(HWND tmpHwnd, int x, int y, int cx, int cy, UINT uFlags = SWP_NOZORDER);

	/**------------------------------------------------------------------------------------
	* 名    称:    
	* 说    明:    <设置控件字体>
	* 参    数:    
	* 返    回:
	*---------------------------------------------------------------------------------------*/
	void SetFont(HWND &hWndTmp, HFONT hfont);

	/**------------------------------------------------------------------------------------
	* 名    称:    <SetImg>
	* 说    明:    <设置控件背景图片的索引值>
	* 参    数:    <int nIndex>：<索引值>
	* 返    回:
	*---------------------------------------------------------------------------------------*/
	void SetImg(int nIndex);

	/**------------------------------------------------------------------------------------
	* 名    称:    <SetText>
	* 说    明:     <设置控件的文本>
	* 参    数:        <CString strText>：<控件文本>
	* 返    回:
	*---------------------------------------------------------------------------------------*/
	void SetText(HWND &hWndTmp, const TCHAR * strText);
	TCHAR* GetText(HWND &hWndTmp);

	/**------------------------------------------------------------------------------------
	* 名    称:    <ShowWnd>
	* 说    明:    <显示或隐藏控件>
	* 参    数:    <bool isSHOW>：<控件是否显示>
	* 返    回:
	*---------------------------------------------------------------------------------------*/
	void ShowWnd(bool isSHOW);

	/**------------------------------------------------------------------------------------
	* 名    称:    <DrawItem>
	* 说    明:    <根据属性画出控件>
	* 参    数:    <WPARAM &wParam>
	*              <LPARAM &lParam>
	* 返    回:
	*---------------------------------------------------------------------------------------*/
	void DrawItem(WPARAM &wParam, LPARAM &lParam);

	/**------------------------------------------------------------------------------------
	* 名    称:    <CreateControl>
	* 说    明:     <创建控件>
	* 参    数:        <char *chValue>：<存储配置信息的字符串>
	*                <char* key>：<主键>
	*                <HWND hWnd>：<控件句柄>
	* 返    回:
	*---------------------------------------------------------------------------------------*/
	void CreateControl(char* chValue, char* key, HWND hWnd);
	HWND CreateLable(HWND hParent, const TCHAR* pszText, UINT style = SS_LEFT | SS_CENTERIMAGE, UINT id = 0);

	BOOL Init(HWND hWnd);
	HWND CreateTextEdit(HWND hParent, UINT style = 0, UINT specialStyle = 0, UINT id = 0);
	void SetLimitTextLen(HWND &hWndTmp, UINT length);
	void SetWindowNumber(HWND &hWndTmp, int number);
	int  GetWindowNumber(HWND &hWndTmp);
	double GetDoubleData(HWND &hWndTmp);
	void SetDoubleData(HWND &hWndTmp, double data);
	void DrawItemFrame(bool bHover, bool bFocus);
	bool DealFloat(WPARAM wParam, LPARAM lParam);

	void CreateLableTextEdit(HWND hParent, const TCHAR* pszText, ControlPos pos, UINT id = 0, 
		UINT style = SS_LEFT | SS_CENTERIMAGE,UINT specialStyle = 0);
	void CreateLableTextEditBtn(HWND hParent, const TCHAR* pszText, ControlPos pos, RECT &rect, IOperator *ip, CPopWinCode &tmpPopWinCode,
	CPopWinCodeList &tmpPopupCodeList, CxEdit &tmpEdit, UINT editId = 0, UINT btnId = 1, UINT style = SS_LEFT | SS_CENTERIMAGE,  UINT specialStyle = 0);
	/*CxSpinEdit CreateLableSpin(HWND hParent, const TCHAR* pszText, ControlPos pos, RECT &rect, UINT id = 0,
		UINT style = SS_LEFT | SS_CENTERIMAGE, UINT specialStyle = 0);*/

	TStaticButton CreateSingleButton(HWND hParent, const char* pszText, ControlPos pos, RECT &rect,UINT id = 0);
	TListView CreateListView(HWND &hParent, map<string, int> &tmpMap, ControlPos pos, RECT &rect, TListView &tmpListView,UINT id,
		UINT style, int itemHeight, int headerHeight, UINT exStyle);

	TCombox CreateLableComboBox(HWND hParent, const TCHAR* pszText, vector<string> &tmpVec,RECT &rect, UINT id = 0);

protected:
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI stdProcCtrl(HWND hWnd, UINT uMsg, UINT wParam, LONG lParam);

private:
	/**------------------------------------------------------------------------------------
	* 名    称:    <SetTitleText>
	* 说    明:     <设置控件的文本>
	* 参    数:        <HWND hWnd>：<控件句柄>
	*                <TCHAR* chLabel>：<文本内容>
	*                <HFONT hFont>：<字体样式>
	*                <UINT textFormat>：<文本样式>
	*                <COLORREF rgb>：<字体颜色>
	* 返    回:
	*---------------------------------------------------------------------------------------*/
	void SetTitleText(HWND hWnd, TCHAR* chLabel,HFONT hFont, UINT textFormat, COLORREF rgb);

	/**------------------------------------------------------------------------------------
	* 名    称:    <DrawBitBmp>
	* 说    明:     <添加控件的背景图片>
	* 参    数:     <HWND hwnd>：<控件句柄>
	*               <CString strFilePath>：<图片路径>
	*                <int nCount>：<图片索引总数>
	*               <int nIndex>：<当前索引>
	* 返    回:
	*---------------------------------------------------------------------------------------*/
	void DrawBitBmp(HWND hwnd, const TCHAR * strFilePath, int nCount, int nIndex);

public:
	bool IsCurContrl(int nID);
	void SetEnable(bool isAble);
	TCHAR* GetText();
	int m_nID;               // 控件ID
	HWND m_hWnd;             // 控件句柄
	RECT m_Rect;

	int m_iExStyle;
	HWND m_hParenthWnd;
	LONG m_OldProc;//原来的回调函数地址

	bool m_bHover;
	bool m_bFocus;
	bool m_bTracking;
	bool m_bChecked;
	bool m_bInRect;
	HFONT m_hFont;
	COLORREF m_bkColor;

	TStatic    m_LabelWithEdit;
	TEdit     m_EditWithLabel;

	TStatic    m_LabelWithEditBtn;
	CxEdit     m_EditWithLabelBtn;
	TStaticButton   m_BtnWithEditLabel;
	CPopWinCode     m_PopupContractCode2;
	CPopWinCodeList m_PopupCodeList2;

	TStatic    m_LabelWithSpin;
	CxSpinEdit m_SpinWithLable;

	TStaticButton m_SingleButton;

	TListView m_ListView;

	TStatic tmpLabelWithCb;
	TCombox m_cbWithLable;
	//CProperty			m_CProperty;
};
