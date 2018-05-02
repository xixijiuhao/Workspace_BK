/*******************************************************************************
* �ļ�����:    BaseControl.h
* ��������:    
* �� �� ��:    
* ˵    ��:    
*******************************************************************************/
#pragma once

typedef struct CreateWndStruct
{
	int nWndType;					//��������    
	DWORD dwstyle;					//������ʽ
	DWORD dwExStyle;				//���ڵ���չ���
	LPCTSTR lpszClass;				//ָ��ע��������ָ��
	LPCTSTR lpszName;				//ָ�򴰿����Ƶ�ָ��
	DWORD style;					//���ڷ��
	HWND hwndParent;				//�����ڵľ��
	HMENU hMenu;					//�˵��ľ�������Ӵ��ڵı�ʶ��
	HINSTANCE hInstance;			//Ӧ�ó���ʵ���ľ��
	LPVOID lpCreateParams;          //ָ�򴰿ڵĴ�������
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
* ��    ��:    <BaseControl>
* ˵    ��:     <�ؼ��Ļ��࣬�������ļ��ж�ȡ�ؼ����ԣ����ṩ�ؼ��Ļ�������>
* ��    ��:
* ��    ��:
* ��    ��:   
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
	* ��    ��:  
	* ˵    ��: <���ÿؼ�λ��>
	* ��    ��:
	* ��    ��:
	*---------------------------------------------------------------------------------------*/
	void OnMove();
	//void MoveWindow(int x, int y, int cx, int cy);
	void MoveWindow(int x, int y, int cx, int cy, UINT uFlags = SWP_NOZORDER);
	void MoveWindow(HWND tmpHwnd, int x, int y, int cx, int cy, UINT uFlags = SWP_NOZORDER);

	/**------------------------------------------------------------------------------------
	* ��    ��:    
	* ˵    ��:    <���ÿؼ�����>
	* ��    ��:    
	* ��    ��:
	*---------------------------------------------------------------------------------------*/
	void SetFont(HWND &hWndTmp, HFONT hfont);

	/**------------------------------------------------------------------------------------
	* ��    ��:    <SetImg>
	* ˵    ��:    <���ÿؼ�����ͼƬ������ֵ>
	* ��    ��:    <int nIndex>��<����ֵ>
	* ��    ��:
	*---------------------------------------------------------------------------------------*/
	void SetImg(int nIndex);

	/**------------------------------------------------------------------------------------
	* ��    ��:    <SetText>
	* ˵    ��:     <���ÿؼ����ı�>
	* ��    ��:        <CString strText>��<�ؼ��ı�>
	* ��    ��:
	*---------------------------------------------------------------------------------------*/
	void SetText(HWND &hWndTmp, const TCHAR * strText);
	TCHAR* GetText(HWND &hWndTmp);

	/**------------------------------------------------------------------------------------
	* ��    ��:    <ShowWnd>
	* ˵    ��:    <��ʾ�����ؿؼ�>
	* ��    ��:    <bool isSHOW>��<�ؼ��Ƿ���ʾ>
	* ��    ��:
	*---------------------------------------------------------------------------------------*/
	void ShowWnd(bool isSHOW);

	/**------------------------------------------------------------------------------------
	* ��    ��:    <DrawItem>
	* ˵    ��:    <�������Ի����ؼ�>
	* ��    ��:    <WPARAM &wParam>
	*              <LPARAM &lParam>
	* ��    ��:
	*---------------------------------------------------------------------------------------*/
	void DrawItem(WPARAM &wParam, LPARAM &lParam);

	/**------------------------------------------------------------------------------------
	* ��    ��:    <CreateControl>
	* ˵    ��:     <�����ؼ�>
	* ��    ��:        <char *chValue>��<�洢������Ϣ���ַ���>
	*                <char* key>��<����>
	*                <HWND hWnd>��<�ؼ����>
	* ��    ��:
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
	* ��    ��:    <SetTitleText>
	* ˵    ��:     <���ÿؼ����ı�>
	* ��    ��:        <HWND hWnd>��<�ؼ����>
	*                <TCHAR* chLabel>��<�ı�����>
	*                <HFONT hFont>��<������ʽ>
	*                <UINT textFormat>��<�ı���ʽ>
	*                <COLORREF rgb>��<������ɫ>
	* ��    ��:
	*---------------------------------------------------------------------------------------*/
	void SetTitleText(HWND hWnd, TCHAR* chLabel,HFONT hFont, UINT textFormat, COLORREF rgb);

	/**------------------------------------------------------------------------------------
	* ��    ��:    <DrawBitBmp>
	* ˵    ��:     <��ӿؼ��ı���ͼƬ>
	* ��    ��:     <HWND hwnd>��<�ؼ����>
	*               <CString strFilePath>��<ͼƬ·��>
	*                <int nCount>��<ͼƬ��������>
	*               <int nIndex>��<��ǰ����>
	* ��    ��:
	*---------------------------------------------------------------------------------------*/
	void DrawBitBmp(HWND hwnd, const TCHAR * strFilePath, int nCount, int nIndex);

public:
	bool IsCurContrl(int nID);
	void SetEnable(bool isAble);
	TCHAR* GetText();
	int m_nID;               // �ؼ�ID
	HWND m_hWnd;             // �ؼ����
	RECT m_Rect;

	int m_iExStyle;
	HWND m_hParenthWnd;
	LONG m_OldProc;//ԭ���Ļص�������ַ

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
