#pragma once 

#define WM_NotifyMsg	WM_USER+251

extern CPlugLoad g_PlugLoad;

class COFInputDlg;
struct WinProperty;
//处理切换tab键等
extern bool DisposeKeyDownMsg_1(HWND hwnd, WPARAM key);


typedef vector<COFInputDlg*> vecAllOFInputDlg;
class CSujectKBOrder
{
public:
	CSujectKBOrder();
	~CSujectKBOrder();
private:
	vecAllOFInputDlg m_vecAllDlg;
	CMyLock m_lock;
	string m_strSender;
	string m_strAction;
	string m_strContent;
public:
	//创建
	COFInputDlg* CreateObject(WinProperty wProperty);
	//删除
	void DeleteObject(COFInputDlg*);
	//获取面板
	COFInputDlg*GetOFInputDlg(HWND hwnd);
	//获取所有面板
	vecAllOFInputDlg & GetAllDlg() { return m_vecAllDlg; }
	//收到通知
	void RecNotice(HWND From,HWND To,const char* sender, const char* action, const char* content);
	//
	static bool GetFirstNotice(const string &strContent, char cSplit, NoticeContent &notice);
	//分割字符串
	static bool SplitNotice(const string &strContent, char cSplit, vector<NoticeContent> &vecstr);

	static bool SplitNoticePosDetail(const string &strContent, char cSplit, vector<NoticeContent> &vecstr);

	static bool SplitNoticePosTotal(const string &strContent, char cSplit, vector<NoticeContent> &vecstr);
};

extern CSujectKBOrder *GetOFInputDlg();

//分组回调
extern void OnLinkageAction(HWND From, HWND To, const char* Sender, const char* Action, const char* Content);
//注册热键
extern void RegHotKey(const bool alt, const bool ctrl, const bool shift, const unsigned char vkey, const bool exclusive=true);
extern void RegFrmHotKey();
//热键回调
void __cdecl OnHotKeyDown(const bool Alt, const bool Ctrl, const bool Shift, const unsigned char VKey, const bool Exclusive, HWND source);
//菜单回调
void __cdecl OnMenuItemClick(const unsigned int MenuIndex, const HWND PastLife);
