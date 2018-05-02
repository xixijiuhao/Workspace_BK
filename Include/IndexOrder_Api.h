#ifndef _INDEX_ORDER_API_
#define _INDEX_ORDER_API_

const wchar_t PLUG_INDEXORDERAPI_NAME[] = L"IndexOrder";
const wchar_t PLUG_INDEXORDERAPI_VERSION[] = L"1.0.0.1";

class AgridxOrderApi
{

public:
	virtual HWND __cdecl CreateIndexWin() = 0;
//	HWND m_interwnd;
	//virtual bool __cdecl Creates(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle/*,HWND& hretwnd*/) = 0;

};


#endif