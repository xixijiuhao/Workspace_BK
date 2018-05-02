#ifndef _MAIN_FRM_API_
#define _MAIN_FRM_API_

const wchar_t PLUG_MAINFRAME_NAME[] = L"MainFrame";
const wchar_t PLUG_MAINFRAME_VERSION[] = L"1.0.0.1";

struct HWND__; typedef HWND__* HWND;
//��������
typedef int THwndIndex;
#define HI_MainFrame	0	//ϵͳ�����ھ��
#define HI_LastQuote	1	//��������󼤻����鴰�ھ��

//�˵�ִ�лص�����,MenuOrder��λ����λ����һ���ֽڱ�ʾ�������˵�(0x00-0x03)���ڶ������ֽڱ�ʾ���ڲ˵���(0x0000-0xffff)�����ĸ��ֽڱ�ʾ��˳��(0x00-0xff)
#define MAKE_MENUORDER(m, g, p) ((m << 24) | (g << 8) | p)

//PastLife��Ϊ�㣬��ʾϵͳ�ڻ�ԭ����
typedef void(__cdecl *TOnMenuItemClick)(const unsigned int MenuIndex, const HWND PastLife);

//ע��ȫ�ֿ�ݼ��Ļص����� VKeyΪVirtual-Key Codes
typedef void(__cdecl *TOnHotKeyDown)(const bool Alt, const bool Ctrl, const bool Shift, const unsigned char VKey, const bool Exclusive, HWND source);

//���������ص�����
typedef void(__cdecl *TOnLinkageAction)(HWND From, HWND To, const char* Sender, const char* Action, const char* Content);

struct TMenuItemEx
{
public:
	unsigned int				MenuOrder;		//���� ���� ���� ������order
	unsigned int				SubOrder;		//���� order (0,1,2...)
	wchar_t						MenuText[21];	//Ex:���� K��  ���� ���  ί�� �ҵ�
	wchar_t						ClassName[51];	//������ ���������� Ex��class TQuoteFrame
	char						sAction[51];	//�������� 
	char						sContent[51];	//��������
};

class IMainFrame
{
public:
	virtual void __cdecl ShowMain() = 0;

	//menuorderΪ��������ʽ������ֵΪʵ��ʹ�õ�menuindex������0��ʾע��ʧ��
	virtual int __cdecl RegMenuItem(const unsigned int menuorder, const wchar_t* menutext, TOnMenuItemClick onclick) = 0;

	virtual bool __cdecl RegHotKey(const bool alt, const bool ctrl, const bool shift, const unsigned char vkey, const bool exclusive, TOnHotKeyDown onkeydown) = 0;
	virtual bool __cdecl UnregHotKey(const bool alt, const bool ctrl, const bool shift, const unsigned char vkey, const bool exclusive, TOnHotKeyDown onkeydown) = 0;

	virtual bool __cdecl PutInner(HWND hwnd) = 0;
	//���ǰΪѡ������hwndΪPutInner�Ĵ��ھ��
	virtual bool __cdecl ActivateOuter(HWND hwnd) = 0;
	
	//ע��״̬���Ӵ��ڣ�ע���Ӵ�����ʾ�߶�Ϊ20��ע��ǰ�����úô��ڿ��
	virtual bool __cdecl RegStatusBarItem(const unsigned int itemorder, HWND hwnd) = 0;

	//2016-05-27��չMainFrame���ܽӿ�
	virtual int __cdecl RegMenuItemEx(const unsigned int menuorder, const wchar_t* menutext, const wchar_t* hotkeytext, TOnMenuItemClick onclick) = 0;

	//��Ԥ��Ĳ����ļ����貼�� index��0��ʼ
	virtual void __cdecl ResetLayout(const int index) = 0;

	//�ر�Ӧ�ó���
	virtual void __cdecl QuitApplication() = 0;

	//�����ͻָ�Ӧ�ó���
	virtual void __cdecl LockApplication() = 0;
	virtual void __cdecl RestoreApplication() = 0;

	//��������
	virtual bool __cdecl PutInnerEx(HWND hwnd, TOnLinkageAction linkage) = 0;
	virtual void __cdecl Linkage(HWND hwnd, const char* Sender, const char* Action, const char* Content) = 0;

	//���κʹ򿪿�ܴ���
	virtual void __cdecl DisableFrames() = 0;
	virtual void __cdecl EnableFrames() = 0;

	//�ǿ�ܴ��ڵ������ӿ�
	virtual bool __cdecl AddLinkage(HWND hwnd, int group, TOnLinkageAction linkage) = 0;
	virtual bool __cdecl DelLinkage(HWND hwnd) = 0;
	virtual bool __cdecl UpdateLinkage(HWND hwnd, int group) = 0;

	//���ִ��ڶ�ȡ�ͱ������� ���治����1000���ȵ��ַ���
	virtual bool __cdecl GetProperty(HWND hwnd, char* prop, int size) = 0;
	virtual bool __cdecl SetProperty(HWND hwnd, const char* prop) = 0;

	//ע��״̬���Ӵ��ڣ�ע���Ӵ�����ʾ�߶�Ϊ20��ע��ǰ�����úô��ڿ�ȣ�align��1-��2-�ң�RegStatusBarItemEx���� Ĭ�϶�����
	virtual bool __cdecl RegStatusBarItemEx(const unsigned int itemorder, HWND hwnd, unsigned int align) = 0;
	
	//����/��ʾ��������������ȫ��/�ָ���
	virtual void __cdecl SwitchTitleBarShow() = 0;

	//menuorderΪ��������ʽ������ֵΪʵ��ʹ�õ�menuindex������0��ʾע��ʧ��
	virtual int __cdecl AddMenuItemEx(const unsigned int menuorder, TMenuItemEx data[], int nCount) = 0;

	//��ȡ��ش��ھ�� 
	virtual HWND __cdecl GetMainFrameWin(THwndIndex nHwndType) = 0;
};


#endif