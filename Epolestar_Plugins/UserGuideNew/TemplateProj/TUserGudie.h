#ifndef _T_USER_GUIDE_H
#define _T_USER_GUIDE_H

using namespace Gdiplus;

#define BottomLeft_Btn    0x000001
#define BottomCenter_Btn  0x000002
#define BottomRight_Btn   0x000004
#define Inside_LastBtn    0x000008
#define Inside_NextBtn    0x000010

#define totalImgNum  11
class TUserGuide :public TIxFrm
{
public:
	TUserGuide();
	void ShowFrm();
	static void __cdecl OnMenuClk(const unsigned int MenuIndex, const HWND PastLife);
	static void OnHotKeyDown(const bool Alt, const bool Ctrl, const bool Shift, const unsigned char VKey, const bool Exclusive, HWND source);
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnPaint();
	void OnCreate();
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnMouseLeave(WPARAM wParam, LPARAM lParam);
	void DrawCloseBtn(TMemDC* pmemdc);
	void OnClickBtn(WPARAM wParam, LPARAM lParam);
private:
	void SetImgPath(int index);
	void InitImgBtnRect();
	void GetImgPath(const wchar_t* pszPath);

	void Init();
	void SetBtnPos();
	void SetCursorStyle(POINT pt);
	void OnSelLayout(POINT pt);
private:
	TStaticButton  m_btnAction[5];
private:
	ULONG_PTR gdiplusStartupToken;

	TCHAR m_szImgName[totalImgNum][10];
	TCHAR  m_szImgFilePath[200];
	vector<RECT> m_vecNectStepRc;
	vector<RECT> m_vecLastStepRc;
	vector<RECT> m_vecLayoutSelRc;
	Gdiplus::Image*  m_Img;

	int      m_iFrmWidth;
	int		 m_iFrmHeight;
	Gdiplus::Rect m_frmRect;
	int      m_iImgIndex = 0;
	bool	m_bhoverclose = false;
	bool	m_bMouseTrack = false;
	RECT	m_rcClose;
	RECT    m_rcRedraw;
	bool    m_bRedraw = false;
	COLORREF				m_colorBK = RGB(240, 240, 240);
	RECT	m_rcBottomBtn[3];
	RECT    m_rcLink;

	double   m_dZoomOutPercent;
	enum{
		ID_Btn1,  //左下
		ID_Btn2,  //中间
		ID_Btn3,  //右下
		ID_Btn4,  //说明 上一步
		ID_Btn5,  //说明 下一步
	};
};

#endif