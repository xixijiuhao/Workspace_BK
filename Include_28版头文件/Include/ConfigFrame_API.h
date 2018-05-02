#pragma once

//该插件导出IConfigFrame接口
#define PLUG_CONFIGFRAME_NAME 		L"ConfigFrame"	//插件名
#define PLUG_CONFIGFRAME_VERSION	L"1.0.0.1"		//插件版本信息

struct HWND__;
typedef HWND__* HWND;

//配置窗口的创建函数，返回值为该窗口句柄
//	sub_title	: 配置窗口上被点击的子菜单标题
//	parent		: 父窗口
typedef HWND (__stdcall * dlg_create_func)(const wchar_t * /*sub_title*/, const HWND /*parent*/);


///////////////////////////////////////////////////////////////////////////////////////////////////////
enum TConfigMainType
{
	cmtNone	= -1, //无效
	cmtGeneral	, //常规
	cmtQuote	, //行情
	cmtTrade	, //交易
	cmtProgram	, //程序化
	cmtAdvance	, //高级
	cmtExpand	  //扩展
};

//配置框架接口
class IConfigFrame
{
public:
	//注册配置窗口到指定的导航菜单中
	//输入参数：
	//	sub_title	: 配置窗口在子导航菜单中显示的标题，同一主菜单下sub_title不能重复, 同一主菜单下title相同则后注册的会失败
	//	create_func	: 配置窗口创建函数
	//	main_type	: 主导航菜单类型，不在此范围内的值会自动转为cmtExpand类型
	//	sub_index	: 子导航菜单索引，索引从0开始，如果索引已被占用则会自动为自索引加一
	//	right		: 注册者的权限，权限验证不通过的主菜单类型会自动转为cmtExpand类型
	//返回值：
	//	返回菜单ID，如果返回值为0，则注册失败，注销的时候需要将这个ID填入参数
	virtual unsigned int __stdcall reg_config_dlg(const wchar_t * sub_title, const dlg_create_func create_func, TConfigMainType main_type, int sub_index, const char * right = "") = 0;

	//显示配置框架，并切换到指定页面
	//输入参数：
	//	sub_title	: 配置窗口在子导航菜单中显示的标题，如果sub_title为空则切到上次退出时的标签
	//	main_type	: 主导航菜单类型，如果主标签类型为cmtNone，则在所有子标签中查找sub_title
	//	main_type_mask:主导航菜单各项显示与否的位图
	virtual void __stdcall show(const wchar_t * sub_title = L"", const TConfigMainType main_type = cmtNone, const unsigned char main_type_mask = 0xFF) = 0;
	
	//在配置框架上显示配置项帮助信息
	//输入参数：
	//	hint		: 要显示的内容
	virtual void __stdcall set_hint(const wchar_t * hint) = 0;

	//注销菜单，参数为reg_config_dlg的返回值
	virtual void __stdcall unreg_config_dlg(unsigned int menu_id) = 0;
};