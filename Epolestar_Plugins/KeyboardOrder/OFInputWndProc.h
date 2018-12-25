#pragma once 

#define WM_NotifyMsg	WM_USER+251

extern CPlugLoad g_PlugLoad;

class COFInputDlg;
struct WinProperty;
//�����л�tab����
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
	//����
	COFInputDlg* CreateObject(WinProperty wProperty);
	//ɾ��
	void DeleteObject(COFInputDlg*);
	//��ȡ���
	COFInputDlg*GetOFInputDlg(HWND hwnd);
	//��ȡ�������
	vecAllOFInputDlg & GetAllDlg() { return m_vecAllDlg; }
	//�յ�֪ͨ
	void RecNotice(HWND From,HWND To,const char* sender, const char* action, const char* content);
	//
	static bool GetFirstNotice(const string &strContent, char cSplit, NoticeContent &notice);
	//�ָ��ַ���
	static bool SplitNotice(const string &strContent, char cSplit, vector<NoticeContent> &vecstr);

	static bool SplitNoticePosDetail(const string &strContent, char cSplit, vector<NoticeContent> &vecstr);

	static bool SplitNoticePosTotal(const string &strContent, char cSplit, vector<NoticeContent> &vecstr);
};

extern CSujectKBOrder *GetOFInputDlg();

//����ص�
extern void OnLinkageAction(HWND From, HWND To, const char* Sender, const char* Action, const char* Content);
//ע���ȼ�
extern void RegHotKey(const bool alt, const bool ctrl, const bool shift, const unsigned char vkey, const bool exclusive=true);
extern void RegFrmHotKey();
//�ȼ��ص�
void __cdecl OnHotKeyDown(const bool Alt, const bool Ctrl, const bool Shift, const unsigned char VKey, const bool Exclusive, HWND source);
//�˵��ص�
void __cdecl OnMenuItemClick(const unsigned int MenuIndex, const HWND PastLife);
