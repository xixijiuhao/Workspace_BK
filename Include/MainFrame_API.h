#ifndef _MAIN_FRM_API_
#define _MAIN_FRM_API_

const wchar_t PLUG_MAINFRAME_NAME[] = L"MainFrame";
const wchar_t PLUG_MAINFRAME_VERSION[] = L"1.0.0.1";

struct HWND__; typedef HWND__* HWND;
//窗口类型
typedef int THwndIndex;
#define HI_MainFrame	0	//系统主窗口句柄
#define HI_LastQuote	1	//主窗口最后激活行情窗口句柄

//菜单执行回调函数,MenuOrder高位到低位，第一个字节表示所在主菜单(0x00-0x03)，第二三个字节表示所在菜单组(0x0000-0xffff)，第四个字节表示组顺序(0x00-0xff)
#define MAKE_MENUORDER(m, g, p) ((m << 24) | (g << 8) | p)

//PastLife不为零，表示系统在还原布局
typedef void(__cdecl *TOnMenuItemClick)(const unsigned int MenuIndex, const HWND PastLife);

//注册全局快捷键的回调函数 VKey为Virtual-Key Codes
typedef void(__cdecl *TOnHotKeyDown)(const bool Alt, const bool Ctrl, const bool Shift, const unsigned char VKey, const bool Exclusive, HWND source);

//分组联动回调函数
typedef void(__cdecl *TOnLinkageAction)(HWND From, HWND To, const char* Sender, const char* Action, const char* Content);

struct TMenuItemEx
{
public:
	unsigned int				MenuOrder;		//行情 交易 数据 主窗口order
	unsigned int				SubOrder;		//分组 order (0,1,2...)
	wchar_t						MenuText[21];	//Ex:报价 K线  竖向 点价  委托 挂单
	wchar_t						ClassName[51];	//主窗口 窗口类名称 Ex：class TQuoteFrame
	char						sAction[51];	//联动操作 
	char						sContent[51];	//联动内容
};

class IMainFrame
{
public:
	virtual void __cdecl ShowMain() = 0;

	//menuorder为建议排序方式，返回值为实际使用的menuindex，返回0表示注册失败
	virtual int __cdecl RegMenuItem(const unsigned int menuorder, const wchar_t* menutext, TOnMenuItemClick onclick) = 0;

	virtual bool __cdecl RegHotKey(const bool alt, const bool ctrl, const bool shift, const unsigned char vkey, const bool exclusive, TOnHotKeyDown onkeydown) = 0;
	virtual bool __cdecl UnregHotKey(const bool alt, const bool ctrl, const bool shift, const unsigned char vkey, const bool exclusive, TOnHotKeyDown onkeydown) = 0;

	virtual bool __cdecl PutInner(HWND hwnd) = 0;
	//激活当前为选中区域，hwnd为PutInner的窗口句柄
	virtual bool __cdecl ActivateOuter(HWND hwnd) = 0;
	
	//注册状态栏子窗口，注意子窗口显示高度为20，注册前请设置好窗口宽度
	virtual bool __cdecl RegStatusBarItem(const unsigned int itemorder, HWND hwnd) = 0;

	//2016-05-27扩展MainFrame功能接口
	virtual int __cdecl RegMenuItemEx(const unsigned int menuorder, const wchar_t* menutext, const wchar_t* hotkeytext, TOnMenuItemClick onclick) = 0;

	//从预设的布局文件重设布局 index从0开始
	virtual void __cdecl ResetLayout(const int index) = 0;

	//关闭应用程序
	virtual void __cdecl QuitApplication() = 0;

	//锁定和恢复应用程序
	virtual void __cdecl LockApplication() = 0;
	virtual void __cdecl RestoreApplication() = 0;

	//分组联动
	virtual bool __cdecl PutInnerEx(HWND hwnd, TOnLinkageAction linkage) = 0;
	virtual void __cdecl Linkage(HWND hwnd, const char* Sender, const char* Action, const char* Content) = 0;

	//屏蔽和打开框架窗口
	virtual void __cdecl DisableFrames() = 0;
	virtual void __cdecl EnableFrames() = 0;

	//非框架窗口的联动接口
	virtual bool __cdecl AddLinkage(HWND hwnd, int group, TOnLinkageAction linkage) = 0;
	virtual bool __cdecl DelLinkage(HWND hwnd) = 0;
	virtual bool __cdecl UpdateLinkage(HWND hwnd, int group) = 0;

	//布局窗口读取和保存属性 保存不超过1000长度的字符串
	virtual bool __cdecl GetProperty(HWND hwnd, char* prop, int size) = 0;
	virtual bool __cdecl SetProperty(HWND hwnd, const char* prop) = 0;

	//注册状态栏子窗口，注意子窗口显示高度为20，注册前请设置好窗口宽度，align：1-左，2-右，RegStatusBarItemEx调用 默认都是左
	virtual bool __cdecl RegStatusBarItemEx(const unsigned int itemorder, HWND hwnd, unsigned int align) = 0;
	
	//隐藏/显示标题栏（主界面全屏/恢复）
	virtual void __cdecl SwitchTitleBarShow() = 0;

	//menuorder为建议排序方式，返回值为实际使用的menuindex，返回0表示注册失败
	virtual int __cdecl AddMenuItemEx(const unsigned int menuorder, TMenuItemEx data[], int nCount) = 0;

	//获取相关窗口句柄 
	virtual HWND __cdecl GetMainFrameWin(THwndIndex nHwndType) = 0;
};


#endif