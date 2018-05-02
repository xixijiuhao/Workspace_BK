#pragma once
#pragma comment(lib, "Wininet.lib")

#include "stdafx.h"
#include "afxstr.h"
#include "WinGDI.h"
#include "WinUser.h"
#include <Afxinet.h>
#include "math.h"
#include "time.h"
#include "direct.h"
#include <regex> 
#include "Global.h"

//�ؼ�����Ϸ��Լ��
class CWndCheck:public InputCheck
{
public:
	//������������Ϊ ���������������Ϣ����ǰ�ؼ�
	bool DoCheck(bool bCheck, const char * sErrorInfo, const CWnd * pWnd)
	{
		if (pWnd==nullptr)
			return false;
		return InputCheck::DoCheck(bCheck, sErrorInfo, pWnd->m_hWnd);
	}
	bool ShowError(CWnd * pParent, const char * sCaption, UINT uType)
	{
		HWND hWnd = 0;
		if (pParent)
			hWnd = pParent->m_hWnd;
		return InputCheck::ShowError(hWnd, sCaption, uType);
	}
};

class CWndFun
{
public:
	//����Ļ��Ŀ�괰��
	static void ScreenToWindow(CWnd * pDest, CRect * pRect)
	{
		CRect wRect, cRecr;
		pDest->GetWindowRect(wRect);								//���ھ���
		pDest->GetClientRect(cRecr);								//���ڿͻ�������
		int lBorder = (wRect.Width() - cRecr.Width()) / 2;			//������߿���
		int tBorder = wRect.Height() - cRecr.Height() - lBorder;	//���ڱ��������ϱ߿�߶�

		pRect->OffsetRect(-wRect.left - lBorder, -wRect.top - tBorder);	
	}
	static void ScreenToWindow(CWnd * pDest, CPoint * pPt)
	{
		CRect wRect, cRecr;
		pDest->GetWindowRect(wRect);								//���ھ���
		pDest->GetClientRect(cRecr);								//���ڿͻ�������
		int lBorder = (wRect.Width() - cRecr.Width()) / 2;			//������߿���
		int tBorder = wRect.Height() - cRecr.Height() - lBorder;	//���ڱ��������ϱ߿�߶�

		pPt->Offset(-wRect.left - lBorder, -wRect.top - tBorder);
	}

	//��Դ���ڵ�Ŀ�괰��
	static void WindowToWindow(CWnd * pSouc, CWnd * pDest, CRect * pRect)
	{
		//pSouc->ClientToScreen(pRect);
		//ScreenToWindow(pDest, pRect);
		pSouc->MapWindowPoints(pDest, pRect);
	}

	static void WindowToWindow(CWnd * pSouc, CWnd * pDest, CPoint * pPt)
	{
		//pSouc->ClientToScreen(pPt);
		//ScreenToWindow(pDest, pPt);
		pSouc->MapWindowPoints(pDest, pPt, 1);
	}

	//����Ϊָ�������ڸ������ڵ�λ�ò���
	//���ָ���������丸�����е�λ����Ϣ���ж��Ƿ񶥲㴰�ڣ������������Ƿ�null
	static void GetWindowRectEx(CWnd * pWnd, CRect * pRect)
	{
		CWnd * pParent;
		pWnd->GetWindowRect(pRect);
		pParent=pWnd->GetParent();

		if (pParent!=nullptr)
		{
			::MapWindowPoints(nullptr, pParent->m_hWnd, (LPPOINT)pRect, 2);
			//char sClass[256];
			//::GetClassName(pWnd->m_hWnd, sClass, sizeof(sClass));
			//if (strstr(sClass, "#32770") != nullptr)
			//	pParent->ClientToScreen(pRect);
			//ScreenToWindow(pParent, pRect);
		}
	}

	//�ڸ�������ƽ��
	static void OffsetWindow(CWnd * pWnd, int x, int y)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);
		Rect.OffsetRect(x, y);

		pWnd->MoveWindow(Rect);
	}
	static void OffsetWindow(CWnd * pWnd, CSize Size)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);
		Rect.OffsetRect(Size.cx, Size.cy);

		pWnd->MoveWindow(Rect);
	}

	//��
	static int GetWindowLeft(CWnd * pWnd)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);

		return Rect.left;
	}
	static void SetWindowLeft(CWnd * pWnd, int Value)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);
		Rect.OffsetRect(Value-Rect.left, 0);

		pWnd->MoveWindow(Rect);
	}
	//��
	static int GetWindowTop(CWnd * pWnd)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);

		return Rect.top;
	}
	static void SetWindowTop(CWnd * pWnd, int Value)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);
		Rect.OffsetRect(0, Value-Rect.top);

		pWnd->MoveWindow(Rect);
	}
	//��
	static int GetWindowRight(CWnd * pWnd)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);

		return Rect.right;
	}
	static void SetWindowRight(CWnd * pWnd, int Value)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);
		Rect.OffsetRect(Value-Rect.right, 0);

		pWnd->MoveWindow(Rect);
	}
	//��
	static int GetWindowBottom(CWnd * pWnd)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);

		return Rect.bottom;
	}
	static void SetWindowBottom(CWnd * pWnd, int Value)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);
		Rect.OffsetRect(0, Value-Rect.bottom);

		pWnd->MoveWindow(Rect);
	}
	//��
	static int GetClientWidth(CWnd * pWnd)
	{
		CRect Rect;
		pWnd->GetClientRect(Rect);

		return Rect.Width();
	}
	static int GetWindowWidth(CWnd * pWnd)
	{
		CRect Rect;
		pWnd->GetWindowRect(Rect);

		return Rect.Width();
	}
	static void SetWindowWidth(CWnd * pWnd, int Value)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);
		Rect.right=Rect.left+Value;

		pWnd->MoveWindow(Rect);
	}
	//��
	static int GetClientHeight(CWnd * pWnd)
	{
		CRect Rect;
		pWnd->GetClientRect(Rect);

		return Rect.Height();
	}
	static int GetWindowHeight(CWnd * pWnd)
	{
		CRect Rect;
		pWnd->GetWindowRect(Rect);

		return Rect.Height();
	}
	static void SetWindowHeight(CWnd * pWnd, int Value)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);
		Rect.bottom=Rect.top+Value;

		pWnd->MoveWindow(Rect);
	}
	//����
	static CPoint GetWindowTopLeft(CWnd * pWnd)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);

		return Rect.TopLeft();
	}
	static void SetWindowTopLeft(CWnd * pWnd, int x, int y)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);
		Rect.OffsetRect(x-Rect.left, y-Rect.top);

		pWnd->MoveWindow(Rect);
	}
	static void SetWindowTopLeft(CWnd * pWnd, CPoint Pt)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);
		Rect.OffsetRect(Pt.x-Rect.left, Pt.y-Rect.top);

		pWnd->MoveWindow(Rect);
	}
	//����
	static CPoint GetWindowBottomRight(CWnd * pWnd)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);

		return Rect.BottomRight();
	}
	static void SetWindowBottomRight(CWnd * pWnd, int x, int y)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);
		Rect.OffsetRect(x-Rect.right, y-Rect.bottom);

		pWnd->MoveWindow(Rect);
	}
	static void SetWindowBottomRight(CWnd * pWnd, CPoint Pt)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);
		Rect.OffsetRect(Pt.x-Rect.right, Pt.y-Rect.bottom);

		pWnd->MoveWindow(Rect);
	}

	static void SetWindowVisibleAndEnabeled(CWnd * pWnd, bool IsShow)
	{
		SetWindowVisibleAndEnabeled(pWnd, IsShow, IsShow);
	}
	static void SetWindowVisibleAndEnabeled(CWnd * pWnd, bool IsShow, bool IsEnabel)
	{
		pWnd->EnableWindow(IsEnabel);
		SetWindowVisible(pWnd, IsShow);
	}

	static void SetWindowVisible(CWnd * pWnd, bool IsVisible)
	{
		if (IsVisible)
			pWnd->ShowWindow(SW_SHOW);
		else
			pWnd->ShowWindow(SW_HIDE);
	}

	//���󴰿�
	static void InflateWindow(CWnd * pWnd, int x, int y)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);
		Rect.InflateRect(x, y);

		pWnd->MoveWindow(Rect);
	}
	static void InflateWindow(CWnd * pWnd, CSize Size)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);
		Rect.InflateRect(Size);

		pWnd->MoveWindow(Rect);
	}
	static void InflateWindow(CWnd * pWnd, CRect rect)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);
		Rect.InflateRect(rect);

		pWnd->MoveWindow(Rect);
	}
	static void InflateWindow(CWnd * pWnd, int l, int t, int r, int b)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);
		Rect.InflateRect( l, t, r, b);

		pWnd->MoveWindow(Rect);
	}

	//��С����
	static void DeflateWindow(CWnd * pWnd, int x, int y)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);
		Rect.DeflateRect(x, y);

		pWnd->MoveWindow(Rect);
	}
	static void DeflateWindow(CWnd * pWnd, CSize Size)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);
		Rect.DeflateRect(Size);

		pWnd->MoveWindow(Rect);
	}
	static void DeflateWindow(CWnd * pWnd, CRect rect)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);
		Rect.DeflateRect(rect);

		pWnd->MoveWindow(Rect);
	}
	static void DeflateWindow(CWnd * pWnd, int l, int t, int r, int b)
	{
		CRect Rect;
		GetWindowRectEx(pWnd, &Rect);
		Rect.DeflateRect( l, t, r, b);

		pWnd->MoveWindow(Rect);
	}

	static void GetWorkAreaRect(CRect * pRect)
	{
		SystemParametersInfo(SPI_GETWORKAREA, 0, pRect, 0);
	}
	//���ô������ߵ���Ļ����ƽ�ָ��
	static void MoveToScreenGolden(CWnd * pWnd)
	{
		CRect Rect(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		if (AfxGetApp()->GetMainWnd()!=pWnd)
			AfxGetApp()->GetMainWnd()->GetWindowRect(&Rect);

		int left =  Rect.left + (int)((Rect.Width()  - GetWindowWidth(pWnd))/2);
		int top = Rect.top  + (int)((Rect.Height()*(1-0.618) - GetWindowHeight(pWnd)/2));
		if (top<=Rect.top)
			top = Rect.top  + (int)((Rect.Height() - GetWindowHeight(pWnd))*(1-0.618));
		
		SetWindowTopLeft(pWnd, left, top);
	}
	static void MoveToMainWindowGolden(CWnd * pWnd)
	{	
		HWND hwnd = GetTopParent(pWnd->m_hWnd);

		CRect Rect;
		GetWindowRect(hwnd, &Rect);

		int left = Rect.left + (int)((Rect.Width()  - GetWindowWidth(pWnd))/2);
		int top = Rect.top  + (int)((Rect.Height()*(1-0.618) - GetWindowHeight(pWnd)/2));
		if (top<=Rect.top)
			top = Rect.top  + (int)((Rect.Height() - GetWindowHeight(pWnd))*(1-0.618));

		SetWindowTopLeft(pWnd, left, top);
	}
	static void SetClientRect(CWnd * pWnd, CRect & Rect)
	{
		CRect winRect, cliRect;		
		pWnd->GetWindowRect(winRect);
		pWnd->GetClientRect(cliRect);
		winRect.right  += (Rect.Width()  - cliRect.Width());
		winRect.bottom += (Rect.Height() - cliRect.Height());
		pWnd->MoveWindow(winRect);
	}
	//��������Ӧ��С��Margins��ʾ�ıߵı߾�,bOnlyVisibleChild��ʾֻ���ݿ��ӵ��Դ�������Ӧ��С
	static void DoAuotSize(CWnd * pWnd, MARGINS & Margins, bool bOnlyVisibleChild = true)
	{
		CRect cliRect, mRect;		

		int dxLeft = 0;
		int dxTop  = 0;
		bool bFirst = false;
		CWnd * pClient = pWnd->GetTopWindow();
		while (pClient!=nullptr)
		{
			if (pClient->IsWindowVisible() || !bOnlyVisibleChild)
			{
				pClient->GetWindowRect(mRect);
				if (!bFirst)//��һ���Ӵ��ڵ�ƫ���������������Ӵ��ڵ�ƫ����
				{					
					bFirst = true;
					dxLeft = Margins.cxLeftWidth-mRect.left;
					dxTop  = Margins.cyTopHeight-mRect.top;
				}

				cliRect.UnionRect(cliRect, mRect);

				mRect.OffsetRect(dxLeft, dxTop);
				pClient->MoveWindow(mRect);
			}
			pClient = pClient->GetNextWindow();
		}

		cliRect.right	+= (Margins.cxLeftWidth + Margins.cxRightWidth);
		cliRect.bottom	+= (Margins.cyTopHeight + Margins.cyBottomHeight);

		SetClientRect(pWnd, cliRect);
	}

	static CWnd * GetFirstParentDlg(CWnd * pWnd)
	{
		CWnd * pwnd=pWnd;
		while (pwnd)
		{
			if (pwnd->IsKindOf(RUNTIME_CLASS(CDialog)))
				return pwnd;
			pwnd = pwnd->GetParent();
		}
		return nullptr;
	}

	static HWND GetTopParent(HWND hwnd)
	{
		HWND Hwnd = hwnd;
		while (::GetParent(Hwnd))
			Hwnd = ::GetParent(Hwnd);
		return Hwnd;
	}

	static BOOL ModifyControlStyle(CWnd* pWnd,LPCTSTR lpszClassName,DWORD dwRemove,DWORD dwAdd,DWORD dwRemoveEx,DWORD dwAddEx)
	{
		CWnd* pParentWnd = pWnd->GetParent();
		CFont* pFont = pWnd->GetFont();
		CString strText;
		pWnd->GetWindowText(strText);
		{
			// �ڴ���ӱ���ؼ��������ԵĴ���,��ֻ���������������
		}
		CRect rcWindow;
		pWnd->GetWindowRect(&rcWindow);
		pParentWnd->ScreenToClient(rcWindow);
		DWORD dwNewStyle = (pWnd->GetStyle() & ~dwRemove) | dwAdd;
		DWORD dwNewStyleEx = (pWnd->GetExStyle() & ~dwRemoveEx) | dwAddEx;
		UINT nID = pWnd->GetDlgCtrlID();
		pWnd->ShowWindow(SW_HIDE);
		pWnd->DestroyWindow();
		BOOL bResult = pWnd->CreateEx(dwNewStyleEx,lpszClassName,strText,dwNewStyle,rcWindow,pParentWnd,nID);
		pWnd->SetFont(pFont);
		return bResult;
	}
	//��㴰���Ƿ�ģʽ����
	static bool HasModel()
	{
		HWND hwnd = ::GetActiveWindow();
		HWND NewHwnd=0;
		while (hwnd)
		{
			NewHwnd = ::GetParent(hwnd);
			if ((NewHwnd==AfxGetApp()->GetMainWnd()->m_hWnd) || (NewHwnd==0))
				break;
			hwnd = NewHwnd;
		}
		DWORD WinStyle = GetWindowLong(hwnd, GWL_STYLE);

		return ((WinStyle & DS_MODALFRAME)==DS_MODALFRAME) || ((WinStyle & DS_SYSMODAL)==DS_SYSMODAL) ;
	}
	//��㴰���Ƿ񵯳�����
	static bool HasPopup()
	{
		HWND hwnd = ::GetActiveWindow();
		HWND NewHwnd=0;
		while (hwnd)
		{
			NewHwnd = ::GetParent(hwnd);
			if ((NewHwnd==AfxGetApp()->GetMainWnd()->m_hWnd) || (NewHwnd==0))
				break;
			hwnd = NewHwnd;
		}
		DWORD WinStyle = GetWindowLong(hwnd, GWL_STYLE);

		return (WinStyle & WS_POPUP)==WS_POPUP;
	}
};

//��pParentWnd�����ڲ�ѯ��һ��Wnd_Class���ʵ��������pDestWnd����
#define FIND_WND(pParentWnd, Wnd_Class, pDestWnd)\
{\
	pDestWnd = pParentWnd->GetTopWindow();\
	while (pDestWnd)\
	{\
		if (pDestWnd->IsKindOf(RUNTIME_CLASS(Wnd_Class)))\
			break;\
		pDestWnd = pDestWnd->GetNextWindow();\
	}\
};


class CMenuFun
{
public:
	static void TrimSeparator(CMenu* pMenu)				//����ָ���,ɾ����β���ظ��ķָ���
	{
		if (!pMenu) return;
		bool bOldSeparator = true;
		for(int i = pMenu->GetMenuItemCount() - 1; i>=0; i--)
		{
			UINT uState = pMenu->GetMenuState(i, MF_BYPOSITION);

			bool bCurrSeparator = (uState & MF_SEPARATOR)==MF_SEPARATOR;
			if (bCurrSeparator && (bOldSeparator || i==0))
				pMenu->RemoveMenu(i, MF_BYPOSITION);

			bOldSeparator = bCurrSeparator;
		}
	}
	static void VisableMenuItem(CMenu* pMenu, UINT ID, bool bVisable)
	{
		if (bVisable)
			AddMenuItem(pMenu, ID);
		else
			pMenu->RemoveMenu(ID, MF_BYCOMMAND);
	}
	static void AddMenuItem(CMenu* pMenu, UINT ID)//���Ӳ˵���
	{
		if (!pMenu) return;
		if (pMenu->GetMenuState(ID, MF_BYCOMMAND)==0xFFFFFFFF)			
		{
			CString rStr;
			pMenu->GetMenuString(ID, rStr, MF_BYCOMMAND);
			pMenu->AppendMenu(MF_STRING, ID, rStr.GetString());
		}
	}
	static void AddMenuItemByPos(CMenu* pMenu, UINT Position)//���Ӳ˵���
	{
		if (!pMenu) return;
		if (pMenu->GetMenuState(Position, MF_BYPOSITION)==0xFFFFFFFF)			
		{
			CString rStr;
			int ID = pMenu->GetMenuString(Position, rStr, MF_BYPOSITION);
			pMenu->AppendMenu(MF_STRING, ID, rStr.GetString());
		}
	}
	static bool IsVisableMenuItem(CMenu* pMenu, UINT ID)
	{
		if (!pMenu) return false;
		return pMenu->GetMenuState(ID, MF_BYCOMMAND)!=0xFFFFFFFF;
	}
	static bool IsVisableMenuItemByPos(CMenu* pMenu, UINT Position)
	{
		if (!pMenu) return false;
		return pMenu->GetMenuState(Position, MF_BYPOSITION)!=0xFFFFFFFF;
	}	
	static void EnableMenuItem(CMenu* pMenu, UINT ID, bool bEnable)
	{
		if (!pMenu) return;
		pMenu->EnableMenuItem(ID, MF_BYCOMMAND | (bEnable ? MF_ENABLED : MF_GRAYED));
	}
};


#include "load_rc.h"
class CDraw
{
public:
	static void load_png(UINT png_id, __out CBitmap & bmp)
	{
		CImage img;
		LoadRC::load_png(png_id, img);
		if (img.GetBits() == 0)
			return;
		bmp.Attach(img.Detach());
	}
	//mask=-1��Ϊ��Чֵ
	static void img_list_init_form_png(UINT png_id, int cx, int cy, COLORREF mask, __out CImageList & list)
	{		
		CImage img;
		LoadRC::load_png(png_id, img);
		if (img.GetBits() == 0)
			return;
		CBitmap bmp;
		bmp.Attach(img.Detach());
		if (bmp.m_hObject == 0)
			return;

		BITMAP bmpObj;
		bmp.GetBitmap(&bmpObj);
		UINT flag = (mask == (COLORREF) -1) ? 0 : ILC_MASK;
		switch(bmpObj.bmBitsPixel)
		{
			case 4 : 
			default: flag |= ILC_COLOR4;		break;
			case 8 : flag |= ILC_COLOR8;		break;
			case 16: flag |= ILC_COLOR16;		break;
			case 24: flag |= ILC_COLOR24;		break;
			case 32: flag |= ILC_COLOR32;		break;
		}

		list.Create(cx, cy, flag, 0, 0);
		list.Add(&bmp,  mask);
	}
};


//CArray������������������֧�����Ա����
class CSortSearch
{
private:
	template<typename TYPE, typename ARG_TYPE> static void _QSort(CArray<TYPE, ARG_TYPE> & Items, int iLo, int iHi, int (CALLBACK *pCompareFunc)(TYPE, TYPE))
	{
		int Lo = iLo;
		int Hi = iHi;
		TYPE Word;
		TYPE MidWord;
		MidWord = Items[Lo + (Hi - Lo) / 2];

		do 
		{
			while (pCompareFunc(Items[Lo], MidWord) < 0) Lo++;
			while (pCompareFunc(Items[Hi], MidWord) > 0) Hi--;
			if (Lo <= Hi)
			{
				if (pCompareFunc(Items[Lo], Items[Hi]) != 0)				//��������ȵ���
				{
					Word = Items[Lo];
					Items[Lo] = Items[Hi];
					Items[Hi] = Word;
				}
				Lo++;
				Hi--;
			}
		} while (Lo <= Hi);

		if (Hi > iLo) _QSort(Items, iLo, Hi, pCompareFunc);
		if (Lo < iHi) _QSort(Items, Lo, iHi, pCompareFunc);
	}
public:	
	//��CArray�������򣬱ȽϺ���Ϊ��̬����
	template<typename TYPE, typename ARG_TYPE> 
	static void QSort(CArray<TYPE, ARG_TYPE> & Items, int (CALLBACK *pCompareFunc)(TYPE, TYPE))	
	{
		if (Items.GetSize() > 0)
			_QSort(Items, 0, Items.GetCount()-1, pCompareFunc);
	}
	//��CArray�������򣬱ȽϺ���Ϊ���Ա����
	template<typename TYPE, typename ARG_TYPE, class _Class, typename _pMemberFunc> 
	static void QSort(CArray<TYPE, ARG_TYPE> & Items, _Class * pClass, _pMemberFunc pMemberFunc)			
	{
		if (Items.GetSize() > 0)
		{
			typedef int (CALLBACK * _pCompareFunc)(TYPE, TYPE);
			ACCallback<_pCompareFunc, _Class, _pMemberFunc> mCompareFunc(pClass, pMemberFunc);

			QSort(Items, (_pCompareFunc)mCompareFunc);
		}
	}

	//˳�����
	template<typename TYPE, typename ARG_TYPE>  
	static int OrderSearch(TYPE Item, CArray<TYPE, ARG_TYPE> & Items, bool TopDown=true)// TopDown: true�Զ����£�����Ϊ�Ե�����
	{
		if (TopDown)		//�Զ�����
		{
			for (int i=0; i<Items.GetCount(); i++)
				if (Items[i] == Item)
					return i;
		}
		else				//�Ե�����
		{
			for (int i=Items.GetCount()-1; i>=0; i--)
				if (Items[i] == Item)
					return i;
		}
		return -1;
	}
	//���ֲ���
	template<typename TYPE, typename ARG_TYPE>  
	static int BinarySearch(TYPE Item, CArray<TYPE, ARG_TYPE> & Items, int (CALLBACK *pCompareFunc)(TYPE, TYPE))          
	{		
		int High , Mid , Low , isEnd;
		isEnd = -1;
		Low = 0;
		High = Items.GetCount() - 1;
		while (High >= Low)
		{
			Mid = Low + (High - Low) / 2;

			isEnd = pCompareFunc(Item, Items[Mid]);
			if (isEnd > 0)
				Low = Mid + 1;
			else if (isEnd < 0)
				High = Mid - 1;
			else
				break;
		}

		if (0==isEnd)		//�ڶ��ֵĽ��������˳�����
		{
			for (int i = Mid + 1; i <= High && 0 == pCompareFunc(Item, Items[i]); i++)
				if (Item == Items[i])
					return i;

			for (int i = Mid; i >= Low && 0 == pCompareFunc(Item, Items[i]); i--)
				if (Item == Items[i])
					return i;

			return Mid;
		}
		else
			return Low;
	}
	//���ֲ���
	template<typename TYPE, typename ARG_TYPE, class _Class, typename _pMemberFunc>  
	static int BinarySearch(TYPE Item, CArray<TYPE, ARG_TYPE> & Items, _Class * pClass, _pMemberFunc pMemberFunc)
	{
		typedef int (CALLBACK * _pCompareFunc)(TYPE, TYPE);
		ACCallback<_pCompareFunc, _Class, _pMemberFunc> mCompareFunc(pClass, pMemberFunc);

		//��ָ��ĵ���
		//pClass->*pMemberFunc();

		return BinarySearch(Item, Items, (_pCompareFunc)mCompareFunc);
	}
};