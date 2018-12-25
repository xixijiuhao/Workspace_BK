#pragma once

#include "ACCallback.h"

class IHookExecute
{
public:
	virtual int Execute(WPARAM Msg, void *pData)=0;
};

class CHook
{
private:
	int				m_nHookId;
	HHOOK			m_hHook;
	bool			m_bHooked;
	IHookExecute*	m_pHookExecute;

	LRESULT HookBackProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lResult = ::CallNextHookEx(m_hHook, nCode, wParam, lParam);

		if (nCode == HC_ACTION && m_pHookExecute!=NULL)
			return m_pHookExecute->Execute(wParam, (void *)lParam);

		return lResult;
	}

	template<class _Class, typename _pMemberFunc> 
	bool HookOn(_Class * pClass, _pMemberFunc pMemberFunc)
	{		
		ACCallback<HOOKPROC, _Class, _pMemberFunc>mHookFunc(pClass, pMemberFunc);
		m_hHook = ::SetWindowsHookEx(m_nHookId, (HOOKPROC)mHookFunc, GetModuleHandle(NULL), 0);
		return m_hHook != 0;
	}
	bool HookOff()
	{
		return ::UnhookWindowsHookEx(m_hHook)!=0;
	}
public:
	CHook(int HookId, IHookExecute*	pHookExecute, bool bOn=true)
	{
		m_nHookId		= HookId;
		m_pHookExecute	= pHookExecute;
		SetHooked(bOn);
	}
	~CHook()
	{
		m_bHooked = HookOff();
	}
	void SetHooked(bool Value)
	{
		if (Value)
			m_bHooked = HookOn(this, &CHook::HookBackProc);
		else
			m_bHooked = HookOff();
	}
	bool GetHooked()
	{
		return m_bHooked;
	}
};