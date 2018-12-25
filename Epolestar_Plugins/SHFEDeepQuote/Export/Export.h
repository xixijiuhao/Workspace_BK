#ifndef _T_EXPORT_H_
#define _T_EXPORT_H_



#ifndef SAFE_DELETE
#define SAFE_DELETE(p) { if(p){delete(p);  (p)=NULL;} }
#endif
extern ITradeApi*     g_pTradeApi;
extern ITradeData*    g_pTradeData;
extern IMainFrame*    g_pMainFrame;
extern IStarApi*     g_pStarApi;
extern SHFEDeepQuoteFrm* g_DeepQuoteFrm;
//extern CLoginFrm* g_LoginFrm;
extern ColumnConfigView* m_ColumuConfig;

extern G_COLOREF g_ColorRefData;
extern G_UIFONT  g_FontData;
extern ILanguageApi*		g_pLanguage;
//extern MBLApi* g_pMBLApi;
//extern CMBLSpiHandler * g_pMBLSpiHandler;

#define WM_SHFEMESSAGE						WM_USER + 1
#define WM_LISTMESSGAE						WM_USER + 2
#define SSWM_STATIC_BUTTON_CLICKDOWN		WM_USER + 10
#define SSWM_TAB_SWITCH_FOCUS				WM_USER + 11
#define SSWM_COMBOX_SELECT					WM_USER + 12
#define SSWM_COMBOX_CHECK_CHANAGE           WM_USER + 13
#define SSWM_PINEDIT_UP                     WM_USER + 14
#define SSWM_PINEDIT_DOWN                   WM_USER + 15
#define SSWM_EDIT_VK_RETURN                 WM_USER + 16

#define SSWM_ORDERBUTTON_UP                 WM_USER + 17
#define WM_RESETUSERNO                      WM_USER + 18
#define WM_NOTICE                           WM_USER + 19
#define WM_ONQUOTE                          WM_USER + 20
#define WM_POSITION                         WM_USER + 21
#define WM_SUPERPRICE                       WM_USER + 22
#define WM_ONCALLBACK                       WM_USER + 23
#define WM_ONORDER                          WM_USER + 24
#define WM_ONHEADERINFOCHANGED              WM_USER + 25
#define WM_ONCONTRACTDATAINIT               WM_USER + 26
#define WM_ONMONEY                          WM_USER + 27
#define WM_ONMATCH                          WM_USER + 28
#define WM_ONFUNCKEYDOWN                    WM_USER + 29
#define WM_ONFUNCKEYUP                      WM_USER + 30
#define WM_ONSETTIPS                        WM_USER + 31
#define WM_COLUMNQUENCE						WM_USER + 32
#define WM_QUOTELOGIN						WM_USER + 33

#define WM_UPDATEQUOTE						WM_USER + 34
#define WM_RECEIVEQUOTEDATA					WM_USER + 35

#define DrawWideText(hdc,text,rect,dword)		\
{				\
	char szmbyte[1024]; sprintf_s(szmbyte,"%s",text);	\
	DWORD dwsize = 1024; wchar_t sztext[1024];	\
	MByteToWChar(szmbyte, sztext, dwsize);		\
	DrawText(hdc, sztext, wcslen(sztext), &rect, dword);	\
}
bool MByteToWChar(LPCSTR lpcszStr, LPWSTR lpwszStr, DWORD dwSize);

#endif