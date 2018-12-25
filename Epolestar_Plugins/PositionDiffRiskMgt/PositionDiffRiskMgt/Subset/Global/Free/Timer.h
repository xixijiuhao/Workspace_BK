#pragma once

//必须配合CWnd的OnTimer()事件使用，定时器控件,m_hWnd和m_nIDEvent相同则认为是相同的控件
class CTimer
{
private:
	HWND m_hWnd;
	UINT m_nID;

	bool m_bOpened;
	int  m_iInterval;
public:
	CTimer():m_bOpened(false), m_iInterval(1000),m_hWnd(0),m_nID(0)
	{
	}
	void Init(HWND hWnd, UINT nID)
	{
		m_hWnd	= hWnd;
		m_nID	= nID;
		UpdateTimer();
	}
	void Open()
	{
		if (true == m_bOpened)
			return;
		m_bOpened = true;
		UpdateTimer();
	}
	void Close()
	{
		if (false == m_bOpened)
			return;
		m_bOpened = false;
		UpdateTimer();
	}
	bool IsOpened()
	{
		return m_bOpened;
	}
	void SetInterval(DWORD Value=1000)
	{
		if (m_iInterval == Value)
			return;
		m_iInterval = Value;
		UpdateTimer();
	}
	DWORD GetInterval()
	{
		return m_iInterval;
	}
	bool UpdateTimer()
	{
		if (!(m_hWnd && m_nID))
			return false;

		KillTimer(m_hWnd, m_nID);
		if (0 != m_iInterval && m_bOpened)
			return SetTimer(m_hWnd, m_nID, m_iInterval, NULL) != 0;
		else
			return false;
	}
	HWND GetWnd()
	{
		return m_hWnd;
	}
	UINT GetID()
	{
		return m_nID;
	}
	bool operator==(CTimer & Timer)
	{
		return (Timer.GetWnd() == m_hWnd && Timer.GetID() == m_nID);
	}
};

//定时锁定
#include "LockDlg.h"
#include "ForeignTrade.h"
#include "Singlton.h"

extern CForeignTrade *g_pForeignTrade;

//需要防止创建太早，要不然不会调用WM_TIME处理函数，主框架创建完成之后在创建就可以了
class CTimeLock:public CWnd, public Singleton<CTimeLock>
{							
private:
	enum { IDD = IDD_TimeLock };

	CPoint	m_Pt;
	bool	m_bAutoLock;
	bool	m_Locked;
	int		m_nCurLockSpace;
	int		m_nMaxLockSpace;
	CTimer  m_Timer;

	//是否工作中
	bool IsWorking(MSG * pMsg)
	{		
		bool bWorking;
		if (pMsg->message == WM_MOUSEMOVE)
		{
			bWorking = (m_Pt != pMsg->pt)==TRUE;
			m_Pt = pMsg->pt;
		}
		else
			bWorking = (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST ||
			pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST);

		return bWorking;
	}
	//锁定
	void DoLock()
	{
		m_Locked = true;

		CArray<HWND> arrWindow;		//锁定时需要隐藏的窗口

		//隐藏所有顶层窗口
		HWND hwnd = ::GetTopWindow(0);
		while(hwnd)
		{ 
			if ((theApp.IsChildOfMain(hwnd) || theApp.IsMain(hwnd)) && hwnd != theApp.GetMainWnd()->m_hWnd && ::IsWindowVisible(hwnd)) 
			{
				arrWindow.Add(hwnd);
				::ShowWindow(hwnd, SW_HIDE);
			}
			hwnd = ::GetNextWindow(hwnd, GW_HWNDNEXT);
		}

		//隐藏主窗口
		theApp.GetMainWnd()->ShowWindow(SW_HIDE);

		//显示锁定窗口
		CLockDlg LockDlg;
		LockDlg.DoModal();

		//先显示主窗口
		theApp.GetMainWnd()->ShowWindow(SW_SHOW);
		//再显示锁定时隐藏掉的窗口
		for (int i=0; i<arrWindow.GetCount(); i++)
			::ShowWindow(arrWindow.GetAt(i), SW_SHOW);

		m_Locked = false;
	}
public:
	void SetAutoLock(bool Value)
	{
		if (m_bAutoLock == Value)
			return;

		m_bAutoLock = Value;

		if (!m_bAutoLock)
			m_Timer.Close();
	}
	bool IsAutoLock()
	{
		return m_bAutoLock;
	}
	void SetMaxLockSpace(int Value)
	{
		if (Value>0)
			m_nMaxLockSpace = Value;
	}
	int GetMaxLockSpace()
	{
		return m_nMaxLockSpace;
	}
	
protected:	 
	CTimeLock():m_bAutoLock(false)
		, m_nCurLockSpace(0)
		, m_nMaxLockSpace(10)
		, m_Locked(false)
	{
		CRect rect(0,0,0,0);
		if (CWnd::Create(NULL, NULL, 0, rect, theApp.GetMainWnd(), IDD))//创建关联窗口
		{
			m_Timer.SetInterval(60*1000);								//1分钟判断一次
			m_Timer.Init(m_hWnd, 1);
		}
	}	
    friend  class Singleton<CTimeLock>;
public:
	~CTimeLock()
	{
		DestroyWindow();
	};
	//消息判断
	bool DoMessage(MSG * pMsg)
	{
		if (m_Locked || g_pForeignTrade->GetInitCount() < 1)//已锁定或没有初始化成功
			return true;

		if (m_bAutoLock && IsWorking(pMsg) && m_Timer.IsOpened())
			m_Timer.Close();
		else if (m_bAutoLock && ! m_Timer.IsOpened())
		{
			m_nCurLockSpace = 0;
			m_Timer.Open();
		}
		return false;
	}
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT_PTR nIDEvent)
	{
		//是否到达锁定时间
		m_nCurLockSpace++;
		if (m_nCurLockSpace >= m_nMaxLockSpace)
		{			
			m_Timer.Close();
			m_nCurLockSpace = 0;

			DoLock();
		}

		CWnd::OnTimer(nIDEvent);
	}
};

//消息映射
#include "StdAfx.h"

BEGIN_MESSAGE_MAP(CTimeLock, CWnd)
	ON_WM_TIMER()
END_MESSAGE_MAP()