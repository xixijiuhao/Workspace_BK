/**************************************************************************
*   ACCallback.h
*   Helper class of Member function callback mechanism
*	将类的成员函数转为回调函数
**************************************************************************/
#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif 

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#include <windows.h>

#pragma pack(push, 1)
struct _ACCallbackOpCodes
{
	unsigned char tag;// CALL 0xE8
	LONG_PTR offset;  // offset (dest - src - 5, 5=sizeof(tag + offset))
	LONG_PTR _this;   // a this pointer
	LONG_PTR _func;   // pointer to real member function address
};
#pragma pack(pop)

static __declspec( naked ) int STDACJMPProc()
{
	_asm
	{
		POP ECX  
		MOV EAX, DWORD PTR [ECX + 4] // func
		MOV ECX, [ECX]               // this  
		JMP EAX
	}
}

static LONG_PTR CalcJmpOffset(LONG_PTR Src, LONG_PTR Dest)
{
	return Dest - (Src + 5);
}

/* 
_TPStdFunc:		a type of function pointer to API or Callbacks, *MUST* be _stdcall
				一个API函数或系统回调函数指针的类型；转换后的结果类型
_TClass:		the class who owns the callback function. The caller should only own the _stdcall function pointer
					LIFE TIME: It is important to keep the ACCallback object alive until the CALLBACK is not required!!!
				包含_TPMemberFunc所指函数的类；参与转换的类
_TPMemberFunc:	a type of function pointer to class member function, 
					*MUST* be the *DEFAULT* calling conversation, *NO* prefix should be added,
					that is, using ECX for "this" pointer, pushing parameters from right to left, 
					and the callee cleans the stack.
				一个类成员函数指针的类型；类成员函数的地址
*/
template<typename _TPStdFunc, class _TClass, typename _TPMemberFunc>
class ACCallback
{
public:
	_TClass *m_pThis;
	_TPMemberFunc m_pFunc;

private:
	_TPStdFunc m_pStdFunc;

	void MakeCode()
	{
		if (m_pStdFunc) 
			::VirtualFree(m_pStdFunc, 0, MEM_RELEASE);
		m_pStdFunc = (_TPStdFunc)::VirtualAlloc(nullptr, sizeof(_ACCallbackOpCodes), MEM_COMMIT, PAGE_EXECUTE_READWRITE);

		_ACCallbackOpCodes *p = (_ACCallbackOpCodes *)m_pStdFunc;
		p->_func  = *(LONG_PTR *)&m_pFunc;
		p->_this  = (LONG_PTR)m_pThis;
		p->tag    = 0xE8;
		p->offset = CalcJmpOffset((LONG_PTR)p, (LONG_PTR)STDACJMPProc);
	}

public:
	ACCallback<_TPStdFunc, _TClass, _TPMemberFunc>()
	{
	}
	ACCallback<_TPStdFunc, _TClass, _TPMemberFunc>(_TClass* pThis,	_TPMemberFunc pFunc)
	{
		m_pFunc = pFunc;
		m_pThis = pThis;
		m_pStdFunc = nullptr;
		MakeCode();
	}
	void Assign(_TClass* pThis,	_TPMemberFunc pFunc)
	{
		m_pFunc = pFunc;
		m_pThis = pThis;
		m_pStdFunc = nullptr;
		MakeCode();
	}
	virtual ~ACCallback<_TPStdFunc, _TClass, _TPMemberFunc>()
	{
		::VirtualFree(m_pStdFunc, 0, MEM_RELEASE);
	}
	operator _TPStdFunc()
	{
		return m_pStdFunc;
	}
};

/*示例
class CTestCallback
{
private:
	typedef void (CTestCallback::*CLASSTIMERPROC)(HWND, UINT, UINT_PTR, DWORD);
	void OnTimer (HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	ACCallback<TIMERPROC, CTestCallback, CLASSTIMERPROC> m_DOnTimer;
}
*/
//调用时，只要这样写：
///* 初始化回调结构 */
//m_DOnTimer.Assign(this, &CTestCallback::OnTimer);
//m_uid = ::SetTimer( nullptr, 0, 1000, m_DOnTimer);
