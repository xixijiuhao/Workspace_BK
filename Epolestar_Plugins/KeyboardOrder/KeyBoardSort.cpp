#include "PreInclude.h"
//#include "string_ex.h"

#define CtrlHInterval 6

using namespace std;

extern G_UIFONT g_FontData;			//字体
extern G_COLOREF g_ColorRefData;	//色彩

void  SetCenterShow(HWND hWnd)
{
	RECT rect, rc_parent;
	HWND parent = hWnd;
	while (::GetParent(parent))
		parent = ::GetParent(parent);
	GetWindowRect(parent, &rc_parent);
	GetWindowRect(hWnd, &rect);

	SetWindowPos(hWnd, HWND_TOPMOST, (rc_parent.left + rc_parent.right + rect.left - rect.right) / 2, (rc_parent.top + rc_parent.bottom + rect.top - rect.bottom) / 2, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
}

KeyBoardSort::KeyBoardSort()
{
	LoadParam();
}

KeyBoardSort::~KeyBoardSort()
{
	if (IsWindow(m_Hwnd))
		DestroyWindow(m_Hwnd);
}
void KeyBoardSort::Regist(IKeyOrderCfg* p)
{
	if (p)
	{
		m_vNotify.insert(p);
		C_KeyBoardSeqCfg cfg;
		cfg.nFieldCount = m_iOrderFieldCount;
		cfg.pOrderField = m_pOrderField;
		p->OnKeyBoardSeqChange(&cfg);
	}
}
void KeyBoardSort::UnRegist(IKeyOrderCfg* p)
{
	if (m_vNotify.size() == 0)
		return;
	set<IKeyOrderCfg*>::iterator it = m_vNotify.find(p);
	if (it != m_vNotify.end())
		m_vNotify.erase(p);
}
LRESULT KeyBoardSort::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CREATE			: OnCreateWindow();					break;
		case WM_PAINT			: OnPaint(wParam, lParam);			break;
		case WM_LBUTTONDOWN		: OnLButtonDown(wParam, lParam);	break;
		case WM_COMMAND			: OnCommond(wParam, lParam);		break;
		case WM_DESTROY			:									break;
		case WM_CTLCOLORSTATIC	: return (LONG)m_hStaticBKBrush;	break;
		default					: return NOT_PROCESSED;				break;
	}
	return PROCESSED;
}

void KeyBoardSort::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	CreateFrm(classname, parent, style, exstyle);
}


void KeyBoardSort::OnCreateWindow()
{
	m_hStaticBKBrush		= g_ColorRefData.GetHbrWhite();//背景色
	const int nLeftStart	= 14;		//统一左侧起点
	const int nNormalHeight = 22;		//统一高度
	const int nButtonWidth	= 62;
	const int nEnd			= 230;
	const int nHeightStart	= 26;

	HINSTANCE hInstance = GetModuleHandle(NULL);
	m_hControl[KS_Default_Button]	= CreateWindowEx(0, TEXT("button"), LoadResString(IDS_Seq_Default).c_str(), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, nEnd - nButtonWidth, nHeightStart + 10, nButtonWidth, nNormalHeight, m_Hwnd, (HMENU)KS_Default_Button, hInstance, NULL);
	m_hControl[KS_Up_Button]		= CreateWindowEx(0, TEXT("button"), LoadResString(IDS_Seq_Up).c_str(), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, nEnd - nButtonWidth, nHeightStart + 50, nButtonWidth, nNormalHeight, m_Hwnd, (HMENU)KS_Up_Button, hInstance, NULL);
	m_hControl[KS_Down_Button]		= CreateWindowEx(0, TEXT("button"), LoadResString(IDS_Seq_Down).c_str(), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, nEnd - nButtonWidth, nHeightStart + 80, nButtonWidth, nNormalHeight, m_Hwnd, (HMENU)KS_Down_Button, hInstance, NULL);
	m_hControl[KS_Ok_Button]		= CreateWindowEx(0, TEXT("button"), LoadResString(IDS_Seq_OK).c_str(), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, nEnd - nButtonWidth, nHeightStart + 166, nButtonWidth, nNormalHeight, m_Hwnd, (HMENU)KS_Ok_Button, hInstance, NULL);
	m_hControl[KS_Close_Button]		= CreateWindowEx(0, TEXT("button"), LoadResString(IDS_Seq_Close).c_str(), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, nEnd - nButtonWidth, nHeightStart + 196, nButtonWidth, nNormalHeight, m_Hwnd, (HMENU)KS_Close_Button, hInstance, NULL);
	m_hControl[KS_ListBox]			= CreateWindowEx(0, WC_LISTBOX, TEXT("List box"), WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_BORDER, 10, nHeightStart + 10, 150, 218, m_Hwnd, (HMENU)KS_ListBox, GetModuleHandle(NULL), NULL);
	SendMessage(m_hControl[KS_ListBox], LB_SETITEMHEIGHT, 0, 19);


	m_ButtonDefault.Init(m_hControl[KS_Default_Button]);
	m_ButtonUp.Init(m_hControl[KS_Up_Button]);
	m_ButtonDown.Init(m_hControl[KS_Down_Button]);
	m_ButtonOk.Init(m_hControl[KS_Ok_Button]);
	m_ButtonClose.Init(m_hControl[KS_Close_Button]);
	m_ButtonDefault.SetButtonText(LoadResString(IDS_Seq_Default));
	m_ButtonUp.SetButtonText(LoadResString(IDS_Seq_Up));
	m_ButtonDown.SetButtonText(LoadResString(IDS_Seq_Down));
	m_ButtonOk.SetButtonText(LoadResString(IDS_Seq_OK));
	m_ButtonClose.SetButtonText(LoadResString(IDS_Seq_Close));

	//设置控件字体
	for (int i = KS_START; i <= KS_END; i++)
		if (m_hControl[i] != 0)
			SendMessage(m_hControl[i], WM_SETFONT, (WPARAM)g_FontData.GetFontWord13(), (LPARAM)TRUE);

	//获取配置显示
	vector<C_OrderField> TempArray;
	vector<C_OrderField> arrtempofPrice;
	vector<C_OrderField> arrtempofTriggerPrice;
	vector<C_OrderField> arrtempofIncrePrice;
	C_OrderField tempof;
	C_OrderField tempSeleCode;//选择合约
	GetOrderSortCfg(TempArray, arrtempofPrice, arrtempofTriggerPrice, arrtempofIncrePrice, tempof, tempSeleCode);
	for (vector<C_OrderField>::size_type i = 0; i < TempArray.size(); i++)
	{
		C_OrderField &of = TempArray[i];
		if (CConvert::ReadVersionType() == 0 &&
			(of.id == IAF_VALIDTYPE || of.id == IAF_VALIDTIME || of.id == IAF_ORDERTYPE || of.id == IAF_MAXQTY || of.id == IAF_MINQTY))
			continue;

		ListBox_SetItemData(m_hControl[KS_ListBox], ListBox_AddString(m_hControl[KS_ListBox], GetSortWinText(of.id).c_str()), i);
	}
	::SetWindowPos(m_Hwnd, HWND_TOPMOST, 0, 0, 240, nHeightStart + 230, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOREPOSITION);
	SetCenterShow(m_Hwnd);
}
void KeyBoardSort::OnCommond(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case KS_Default_Button:
		{
			MoveItemByString(LoadResString(IDS_STR_STATIC_UserNo), 0);//客户账号
			//MoveItemByString("定单类型",1);
			MoveItemByString(LoadResString(IDS_STRING_ChangeCode), 1);//合约代码
			//MoveItemByString("合约代码2",3);
			MoveItemByString(LoadResString(IDS_STR_STATIC_Direct), 2);//买/卖
			MoveItemByString(LoadResString(IDS_STR_STATIC_Offset), 3);//开平
			//MoveItemByString("止损价格",6);
			MoveItemByString(LoadResString(IDS_STR_STATIC_OrderPrice), 4);//委托价格
			MoveItemByString(LoadResString(IDS_STR_STATIC_OrderQty), 5);//委托数量
			MoveItemByString(LoadResString(IDS_STR_STATIC_TrigMode), 6);//触发方式
			MoveItemByString(LoadResString(IDS_STR_STATIC_TrigCon), 7);//条件
			MoveItemByString(LoadResString(IDS_STR_STATIC_TrigPrice), 8);//触发价格
			MoveItemByString(LoadResString(IDS_STR_STATIC_MultiQty), 9);//数量倍率

			MoveItemByString(LoadResString(IDS_STR_IncreQty), 10);//数量增量
			MoveItemByString(LoadResString(IDS_STR_IncrePrice), 11);//价格增量
			MoveItemByString(LoadResString(IDS_STR_QtyCount), 12);//下单次数

			MoveItemByString(LoadResString(IDS_STR_STATIC_OrderType), 13);//定单类型
			MoveItemByString(LoadResString(IDS_STR_STATIC_ValidType), 14);//有效类型
			MoveItemByString(LoadResString(IDS_STR_STATIC_ValidDate), 15);//有效日期

			MoveItemByString(LoadResString(IDS_STR_STATIC_MaxQty), 16);//最大量
			MoveItemByString(LoadResString(IDS_STR_STATIC_MinQty), 17);//最小量
		}
		break;
		case KS_Up_Button:
		{
			//获得当前选中项，向上移动一行
			int sel = ListBox_GetCurSel(m_hControl[KS_ListBox]);
			if (sel <= 0)
				return;
			DWORD data = (DWORD)ListBox_GetItemData(m_hControl[KS_ListBox], sel);
			char str[101] = { 0 };
			ListBox_GetText(m_hControl[KS_ListBox], sel, str);
			ListBox_DeleteString(m_hControl[KS_ListBox], sel);
			sel--;
			ListBox_InsertString(m_hControl[KS_ListBox], sel, str);
			ListBox_SetItemData(m_hControl[KS_ListBox], sel, data);
			//要继续选中它
			SetFocus(m_hControl[KS_ListBox]);
			ListBox_SetCurSel(m_hControl[KS_ListBox], sel);
		}
		break;
		case KS_Down_Button:
		{
			//获得当前选中项，向上移动一行
			int sel = ListBox_GetCurSel(m_hControl[KS_ListBox]);
			if (sel == -1 || sel >= ListBox_GetCount(m_hControl[KS_ListBox]) - 1)
				return;
			DWORD data = (DWORD)ListBox_GetItemData(m_hControl[KS_ListBox], sel);
			char str[101] = { 0 };
			ListBox_GetText(m_hControl[KS_ListBox], sel, str);
			ListBox_DeleteString(m_hControl[KS_ListBox], sel);
			sel++;
			ListBox_InsertString(m_hControl[KS_ListBox], sel, str);
			ListBox_SetItemData(m_hControl[KS_ListBox], sel, data);
			//要继续选中它
			SetFocus(m_hControl[KS_ListBox]);
			ListBox_SetCurSel(m_hControl[KS_ListBox], sel);
		}
		break;
		case KS_Ok_Button:
		{
			//记录更改后的信息到array中
			vector<C_OrderField> itemArray;

			//获取原始
			vector<C_OrderField> TempArray;
			vector<C_OrderField> arrtempofPrice;
			vector<C_OrderField> arrtempofTriggerPrice;
			vector<C_OrderField> arrtempofIncrePrice;
			C_OrderField tempof;
			C_OrderField tempSeleCode;//选择合约
			GetOrderSortCfg(TempArray, arrtempofPrice, arrtempofTriggerPrice, arrtempofIncrePrice, tempof, tempSeleCode);

			//记录到临时数组中
			C_OrderField of;
			for (int i = 0; i < ListBox_GetCount(m_hControl[KS_ListBox]); i++)
			{
				int data = (int)ListBox_GetItemData(m_hControl[KS_ListBox], i);
				of = TempArray[data];
				itemArray.push_back(of);
			}
			//更新到正式数组中，按顺序
			TempArray.clear();
			for (int i = 0; i < (int)itemArray.size(); i++)
			{
				of = itemArray[i];
				TempArray.push_back(of);
			}

			//把条件选择那一栏插入到最前面
			TempArray.insert(TempArray.begin(), tempof);
			//分数报价部分添加到对应的价格之后
			for (vector<C_OrderField>::iterator it = TempArray.begin(); it != TempArray.end();)
			{
				if (IAF_COMMODITY == it->id)
					it = TempArray.insert(++it, tempSeleCode);
				else if (IAF_PRICE == it->id)
					it = TempArray.insert(++it, arrtempofPrice.begin(), arrtempofPrice.end());
				else if (IAF_TRIGGERPRICE == it->id)
					it = TempArray.insert(++it, arrtempofTriggerPrice.begin(), arrtempofTriggerPrice.end());
				else if (IAF_INCREPRICE == it->id)
					it = TempArray.insert(++it, arrtempofIncrePrice.begin(), arrtempofIncrePrice.end());
				else
					it++;
			}
			SaveParam(TempArray);
			DestroyWindow(m_Hwnd);
		}
		case KS_Close_Button:
			DestroyWindow(m_Hwnd);
			break;
	}

}
void KeyBoardSort::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	POINT pointB;
	pointB.x = GET_X_LPARAM(lParam);
	pointB.y = GET_Y_LPARAM(lParam);
	if (pointB.y < 26)
		SendMessage(m_Hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
}
C_OrderField KeyBoardSort::CreateOF(int id, int width, int height, int type, int align, BOOL bShow)
{
	C_OrderField of;
	of.id = id;
	of.width = width;
	of.height = height;
	of.bShow = bShow ? 1 : 0;
	return of;
}
void KeyBoardSort::LoadDefaultFieldConfig(vector<C_OrderField> &InputCfgArray)
{
	//-------------------------输入区域的配置信息-------------------------------------
	InputCfgArray.empty();

	InputCfgArray.push_back(CreateOF(IAF_TJCHECK, /*"条件选择",*/ 50));//最后一个绘制
	InputCfgArray.push_back(CreateOF(IAF_ACCOUNT, /*"客户账号",*/ 70));
	InputCfgArray.push_back(CreateOF(IAF_COMMODITY, /*"合约代码",*/ 200));
	InputCfgArray.push_back(CreateOF(IAF_SELECODE, /*"选合约",*/ 10));
	InputCfgArray.push_back(CreateOF(IAF_DIRECT, /*"买/卖",*/ 45));
	InputCfgArray.push_back(CreateOF(IAF_OFFSET, /*"开/平",*/ 45));

	InputCfgArray.push_back(CreateOF(IAF_PRICE, /*"委托价格",*/ 85));
	InputCfgArray.push_back(CreateOF(IAF_PRICE_ADD, /*"+", */85));
	InputCfgArray.push_back(CreateOF(IAF_PRICE_FENZI, /*"分子",*/ 85));
	InputCfgArray.push_back(CreateOF(IAF_PRICE_CHUHAO, /*"/",*/ 85));
	InputCfgArray.push_back(CreateOF(IAF_PRICE_FENMU, /*"分母",*/ 85));

	InputCfgArray.push_back(CreateOF(IAF_VOL, /*"委托数量",*/ 60));

	InputCfgArray.push_back(CreateOF(IAF_TRIGGERMODE, /*"触发方式",*/ 70));
	InputCfgArray.push_back(CreateOF(IAF_TRIGGERCONDITION, /*"条件",*/ 40));

	InputCfgArray.push_back(CreateOF(IAF_TRIGGERPRICE, /*"触发价格",*/ 70));
	InputCfgArray.push_back(CreateOF(IAF_TRIGGERPRICE_ADD, /*"+",*/ 85));
	InputCfgArray.push_back(CreateOF(IAF_TRIGGERPRICE_FENZI, /*"分子",*/ 85));
	InputCfgArray.push_back(CreateOF(IAF_TRIGGERPRICE_CHUHAO, /*"/",*/ 85));
	InputCfgArray.push_back(CreateOF(IAF_TRIGGERPRICE_FENMU, /*"分母",*/ 85));

	InputCfgArray.push_back(CreateOF(IAF_VOLBL, /*"数量倍率",*/ 60));

	InputCfgArray.push_back(CreateOF(IAF_INCREVOL, /*"数量增量",*/ 60));			//数量增量
	InputCfgArray.push_back(CreateOF(IAF_INCREPRICE, /*"价格增量",*/ 60));		//价格增量
	InputCfgArray.push_back(CreateOF(IAF_VOLCOUNT, /*"下单次数",*/ 60));			//下单次数  批次

	InputCfgArray.push_back(CreateOF(IAF_INCRERPRICE_CHUHAO, /*"/",*/ 85));			//价格增量  批次
	InputCfgArray.push_back(CreateOF(IAF_INCREPRICE_FENMU, /*"分母",*/ 85));			//价格增量  批次

	InputCfgArray.push_back(CreateOF(IAF_ORDERTYPE,/*"定单类型",*/ 85));	//定单类型  
	InputCfgArray.push_back(CreateOF(IAF_VALIDTYPE, /*"有效类型",*/ 85));	//有效类型  
	InputCfgArray.push_back(CreateOF(IAF_VALIDTIME, /*"有效日期", */90));	//有效日期  

	InputCfgArray.push_back(CreateOF(IAF_MAXQTY, /*"最大量",*/ 85));	//最大量  
	InputCfgArray.push_back(CreateOF(IAF_MINQTY, /*"最小量",*/ 90));	//最小量  
}

void KeyBoardSort::get_param_file(char * buf, size_t len)
{
	GetModuleFileNameA(nullptr, buf, len);
	char * p = strrchr(buf, '\\');
	len = buf + len - p;
	strncpy_s(++p, len - 1, "config\\KeyBoardOrder.Default.pri", len);
}

bool KeyBoardSort::LoadParam(bool bUpdateToUI)
{
	char file[MAX_PATH];
	get_param_file(file, sizeof(file));
	char buf[1024] = {0};
	GetPrivateProfileStringA("Sequence", "fields", "", buf, sizeof(buf), file);

	vector<C_OrderField> arrtemp;
	C_OrderField field;
	char * p = buf;
	char * q = buf;
	while (true)
	{
		if (*p == ',')
		{
			*p = 0;
			field.id = atoi(q);
			arrtemp.push_back(field);			
			q = ++p;
		}
		else if (*p == 0)
		{
			field.id = atoi(q);
			arrtemp.push_back(field);
			break;
		}
		else
			p++;
	}

	vector<C_OrderField> arrDefault;
	vector<C_OrderField> arrFinaly;
	LoadDefaultFieldConfig(arrDefault);						//始终加载默认值 同客户的配置文件比较，客户有的按客户的 没有的按我的，最新的放到最后
	int iCount = (int)arrtemp.size();
	for (int i = 0; i < (int)iCount; i++)					//遍历客户的配置
	{
		for (int j = 0; j < (int)arrDefault.size(); j++)
			if (arrtemp[i].id == arrDefault[j].id)			//找到 用客户的
				arrFinaly.push_back(arrtemp[i]);
	}
	for (int i = 0; i < (int)arrDefault.size(); i++)		//遍历默认的 把客户没有的加到末尾
	{
		bool bfind = true;
		for (int j = 0; j < (int)iCount; j++)				//遍历客户的配置
			if (arrtemp[j].id == arrDefault[i].id)			//找到 不添加	
				bfind = false;
		if (bfind)
			arrFinaly.push_back(arrDefault[i]);
	}

	//最终的结果
	if (m_iOrderFieldCount > 0 && m_pOrderField)
	{
		m_iOrderFieldCount = 0;
		delete[] m_pOrderField;
		m_pOrderField = NULL;
	}

	m_iOrderFieldCount = (int)arrFinaly.size();
	if (arrFinaly.size() > 0)
		m_pOrderField = new C_OrderField[(int)arrFinaly.size()];
	for (int i = 0; i < (int)arrFinaly.size(); i++)
		memcpy(&m_pOrderField[i], &arrFinaly[i], sizeof(C_OrderField));

	return true;
}

bool KeyBoardSort::SaveParam(vector<C_OrderField>&TempArray)
{	
	//补充默认的
	vector<C_OrderField> arrDefault;
	vector<C_OrderField> arrFinaly;
	LoadDefaultFieldConfig(arrDefault);//始终加载默认值 同客户的配置文件比较，客户有的按客户的 没有的按我的，最新的放到最后
	int iCount = (int)TempArray.size();
	for (int i = 0; i < (int)iCount; i++)//遍历客户的配置
	{
		for (int j = 0; j < (int)arrDefault.size(); j++)
			if (TempArray[i].id == arrDefault[j].id)//找到 用客户的
				arrFinaly.push_back(TempArray[i]);
	}
	for (int i = 0; i < (int)arrDefault.size(); i++)//遍历默认的 把客户没有的加到末尾
	{
		bool bfind = true;
		for (int j = 0; j < (int)iCount; j++)//遍历客户的配置
			if (TempArray[j].id == arrDefault[i].id)//找到 不添加	
				bfind = false;

		if (bfind)
			arrFinaly.push_back(arrDefault[i]);
	}
	//////////////////////////////////////////////////////////////////////////
	if (arrFinaly.size() != 0 && m_iOrderFieldCount > 0 && m_pOrderField)
	{
		if (m_iOrderFieldCount > 0 && m_pOrderField)
		{
			m_iOrderFieldCount = 0;
			delete[] m_pOrderField;
			m_pOrderField = NULL;
		}
		m_iOrderFieldCount = (int)arrFinaly.size();
		m_pOrderField = new C_OrderField[arrFinaly.size()];
		for (int i = 0; i < m_iOrderFieldCount; i++)
			memcpy_s(&m_pOrderField[i], sizeof(C_OrderField), &arrFinaly[i], sizeof(C_OrderField));
	}
	else if (arrFinaly.size() != 0 && 0 == m_iOrderFieldCount&&NULL == m_pOrderField)
	{
		m_iOrderFieldCount = arrFinaly.size();
		m_pOrderField = new C_OrderField[TempArray.size()];
		for (int i = 0; i < m_iOrderFieldCount; i++)
			memcpy_s(&m_pOrderField[i], sizeof(C_OrderField), &arrFinaly[i], sizeof(C_OrderField));
	}
	
	//存文件
	char buf[21];
	string str;
	for (int i = 0; i < m_iOrderFieldCount; ++i)
	{
		_itoa_s(m_pOrderField[i].id, buf, 10);
		str.append(buf);
		if (i < m_iOrderFieldCount - 1)
			str.append(1, ',');
	}
	char file[MAX_PATH];
	get_param_file(file, sizeof(file));
	WritePrivateProfileStringA("Sequence", "fields", str.c_str(), file);

	if (m_vNotify.empty())
		return true;

	C_KeyBoardSeqCfg cfg;
	cfg.nFieldCount = m_iOrderFieldCount;
	cfg.pOrderField = m_pOrderField;
	for (set<IKeyOrderCfg*>::iterator it = m_vNotify.begin(); it != m_vNotify.end(); it++)
		(*it)->OnKeyBoardSeqChange(&cfg);

	return true;
}

//移动
void  KeyBoardSort::MoveItemByString(wstring str, int index)
{
	//首先找到这个string对应的是哪个	
	int sel = ListBox_FindStringExact(m_hControl[KS_ListBox], -1, str.c_str());
	if (sel == LB_ERR)
		return;

	DWORD data = (DWORD)ListBox_GetItemData(m_hControl[KS_ListBox], sel);
	ListBox_DeleteString(m_hControl[KS_ListBox], sel);
	ListBox_InsertString(m_hControl[KS_ListBox], index, str.c_str());
	ListBox_SetItemData(m_hControl[KS_ListBox], index, data);
}
//获取配置
void KeyBoardSort::GetOrderSortCfg(vector<C_OrderField>&TempArray, vector<C_OrderField>&arrtempofPrice, vector<C_OrderField> &arrtempofTriggerPrice, vector<C_OrderField> &arrtempofIncrePrice,	C_OrderField&tempof, C_OrderField &tempSeleCode)
{
	TempArray.clear();
	if (0 != m_iOrderFieldCount&& m_pOrderField)
		for (int i = 0; i < m_iOrderFieldCount; i++)
			TempArray.push_back(m_pOrderField[i]);

	//删除条件选择那一栏 和分数报价部分
	vector<C_OrderField>::iterator itbegin = TempArray.begin();
	for (vector<C_OrderField>::iterator it = itbegin; it != TempArray.end();)
	{
		C_OrderField &of = *it;
		if (of.id == IAF_TJCHECK)//条件选择框，那么先删除它,这一列的顺序不允许调整
			tempof = of;
		else if (IAF_SELECODE == of.id)
			tempSeleCode = of;
		else if (IAF_PRICE_ADD == of.id || IAF_PRICE_FENZI == of.id || IAF_PRICE_CHUHAO == of.id || IAF_PRICE_FENMU == of.id)
			arrtempofPrice.push_back(of);
		else if (IAF_TRIGGERPRICE_ADD == of.id || IAF_TRIGGERPRICE_FENZI == of.id || IAF_TRIGGERPRICE_CHUHAO == of.id || IAF_TRIGGERPRICE_FENMU == of.id)
			arrtempofTriggerPrice.push_back(of);
		else if (IAF_INCRERPRICE_CHUHAO == of.id || IAF_INCREPRICE_FENMU == of.id)
			arrtempofIncrePrice.push_back(of);
		else
		{
			it++;
			continue;
		}
		it = TempArray.erase(it);
	}
}

wstring KeyBoardSort::GetSortWinText(int nID)
{
	switch (nID)
	{
		case IAF_ACCOUNT			: return LoadResString(IDS_STR_STATIC_UserNo);			//客户账号
		case IAF_COMMODITY			: return LoadResString(IDS_STRING_ChangeCode);			//合约代码
		case IAF_DIRECT				: return LoadResString(IDS_STR_STATIC_Direct);			//买卖
		case IAF_OFFSET				: return LoadResString(IDS_STR_STATIC_Offset);			//开平
		case IAF_PRICE				: return LoadResString(IDS_STR_STATIC_OrderPrice);		//委托价格  后面加上 + 分子 /分母 四个 这四个要显示必须跟在委托价格之后
		case IAF_VOL				: return LoadResString(IDS_STR_STATIC_OrderQty);		//委托数量
		case IAF_TRIGGERMODE		: return LoadResString(IDS_STR_STATIC_TrigMode);		//触发方式
		case IAF_TRIGGERCONDITION	: return LoadResString(IDS_STR_STATIC_TrigCon);			//触发条件
		case IAF_TRIGGERPRICE		: return LoadResString(IDS_STR_STATIC_TrigPrice);		//触发价格
		case IAF_VOLBL				: return LoadResString(IDS_STR_STATIC_MultiQty);		//数量倍率
		case IAF_VALIDTIME			: return LoadResString(IDS_STR_STATIC_ValidDate);		//有效日期
		case IAF_VOLCOUNT			: return LoadResString(IDS_STR_QtyCount);				//下单次数  批次
		case IAF_INCREVOL			: return LoadResString(IDS_STR_IncreQty);				//数量增量
		case IAF_INCREPRICE			: return LoadResString(IDS_STR_IncrePrice);				//价格增量 
		case IAF_ORDERTYPE			: return LoadResString(IDS_STR_STATIC_OrderType);		//定单类型
		case IAF_VALIDTYPE			: return LoadResString(IDS_STR_STATIC_ValidType);		//有效类型
		case IAF_MAXQTY				: return LoadResString(IDS_STR_STATIC_MaxQty);			//最大量 冰山
		case IAF_MINQTY				: return LoadResString(IDS_STR_STATIC_MinQty);			//最小量 冰山
		default						: return TEXT("");
	}
}
void KeyBoardSort::OnPaint(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HBRUSH		hBKBrush = CreateSolidBrush(g_ColorRefData.GetColorGrayTitle());			//静态文本背景画刷
	HDC hDC = BeginPaint(m_Hwnd, &ps);
	RECT rect, rectClient;
	::GetWindowRect(m_Hwnd, &rect);
	rectClient.left = 0;
	rectClient.top = 0;
	rectClient.right = rect.right - rect.left;
	rectClient.bottom = 26;

	FillRect(hDC, &rectClient, hBKBrush);
	rectClient.bottom = rect.bottom - rect.top;

	SetTextColor(hDC, g_ColorRefData.GetColorWhite());
	SetBkMode(hDC, TRANSPARENT);
	SelectObject(hDC, g_FontData.GetFontWord13());
	RECT rText;
	rText.left = 1;
	rText.top = 5;
	rText.bottom = 20;
	rText.right = rectClient.right - 1;

	DrawText(hDC, LoadResString(IDS_SeqSet).c_str(), LoadResString(IDS_SeqSet).size(), &rText, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	FrameRect(hDC, &rectClient, hBKBrush);
	EndPaint(m_Hwnd, &ps);
}


