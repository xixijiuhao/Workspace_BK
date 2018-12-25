#include "PreInclude.h"

extern HINSTANCE g_hinst;
extern CPlugLoad g_PlugLoad;

CSujectKBOrder::CSujectKBOrder()
{
	
}
CSujectKBOrder::~CSujectKBOrder()
{
	
}
COFInputDlg* CSujectKBOrder::CreateObject(WinProperty wProperty)
{
	COFInputDlg *pDlg = new COFInputDlg(NULL, wProperty);
	if (m_vecAllDlg.size() != 0)
	{
		for (vecAllOFInputDlg::iterator i = m_vecAllDlg.begin(); i != m_vecAllDlg.end(); i++)
			(*i)->SetHotKeyActive(false);	
	}
	//最后有效
	pDlg->SetHotKeyActive(true);
	m_vecAllDlg.push_back(pDlg);
	return pDlg;
}
void CSujectKBOrder::DeleteObject(COFInputDlg*p)
{
	if (p)
	{
		bool bActive = false;
		for (vecAllOFInputDlg::iterator i = m_vecAllDlg.begin(); i != m_vecAllDlg.end(); i++)
		{
			if (p == *i)
			{
				bActive = p->GetHotKeyActive();
				m_vecAllDlg.erase(i);
				break;
			}
		}
		delete p;

		if (bActive&&!m_vecAllDlg.empty())//激活新的Alt快捷键窗口
		{
			vecAllOFInputDlg::iterator iter = m_vecAllDlg.begin() + (m_vecAllDlg.size() - 1);
			(*iter)->SetHotKeyActive(true);
		}
	}
}
COFInputDlg*CSujectKBOrder::GetOFInputDlg(HWND hwnd)
{
	for (vecAllOFInputDlg::iterator i = m_vecAllDlg.begin(); i != m_vecAllDlg.end(); i++)
	{
		if ((*i)->GetHwnd() == hwnd)
			return (*i);
	}
	return NULL;
}
void CSujectKBOrder::RecNotice(HWND From, HWND To, const char* Sender, const char* Action, const char* Content)
{
	if (g_PlugLoad.GetMainFrm() && g_pTradeApi&&g_pTradeData&&g_PlugLoad.GetStarApi())
	{
		for (vecAllOFInputDlg::iterator it = m_vecAllDlg.begin(); it != m_vecAllDlg.end(); it++)
		{
			if (To == (*it)->GetHwnd())//可以直接发
			{
				if (!strcmp(Sender, "PolestarQuote"))//行情 K线合约变化
					(*it)->SetCodeFromQuote(Content, Action);
				else if (!strcmp(Sender, "TradeDisplay"))//列表 改单
				{
					if (!strcmp(Action, W_CSTYLE_MODIFY) || !strcmp(Action, W_CSTYLE_MODIFY_BATCH))//改单 批量改单
					{
						string sKey1 = Content;
						int npos = sKey1.find_first_of('=');
						if (npos > 0)
							sKey1 = sKey1.substr(npos + 1, sKey1.length());
						else
							return;

						const TOrder *pModifyOrder = g_pTradeData->GetOrder(atoi(sKey1.c_str()));
						if (!pModifyOrder)
							return ;
					
						(*it)->ModifyOrderOper(pModifyOrder, !strcmp(Action, W_CSTYLE_MODIFY_BATCH));
					}
					else if (!strcmp(Action, W_BN_DBCLICK) || !strcmp(Action, W_BN_CLICK))
					{
						string sflag = Content;
						int npos = sflag.find(';');
						if (npos >= 0)
						{
							sflag = sflag.substr(0, npos);
							if (sflag == LoadRC::unicode_to_ansi(LoadResString(IDS_PosTotal)))
								(*it)->SetCodeFromPosTotal(Content, Action);
							else if (sflag == LoadRC::unicode_to_ansi(LoadResString(IDS_PosDetails)))
								(*it)->SetCodeFromPosSub(Content, Action);
							else if (sflag == LoadRC::unicode_to_ansi(LoadResString(IDS_CommissionInfo)))
								(*it)->SetCodeFromOrder(Content, Action);
						}
					}
				}
				else if (!strcmp(Sender, "CommonModule"))
				{
					if (!strcmp(Action, WM_MODIFY_RECOVER))		//改单完成 界面恢复				
						(*it)->OnDealModifyRecover();
				}
				break;	
			}
		}	
	}
}
bool CSujectKBOrder::GetFirstNotice(const string &strContent, char cSplit, NoticeContent& notice)
{
	string sContent = strContent;
	string::size_type isplit = sContent.find(cSplit);

	if (string::npos != isplit)
	{
		string strtmp = sContent.substr(0, isplit++);
		string::size_type iIdent = sContent.find('=');
		if (string::npos != iIdent)
		{
			notice.strKey = strtmp.substr(0, iIdent);
			notice.strContent = strtmp.substr(++iIdent, string::npos);
			return true;
		}
		else
			return false;
	}
	else
		return false;
}
bool CSujectKBOrder::SplitNotice(const string &strContent, char cSplit, vector<NoticeContent> &vecstr)
{
	vecstr.clear();
	string sContent = strContent;
	string::size_type isplit = sContent.find(cSplit);
	NoticeContent tmp;
	if (string::npos == isplit)
	{
		string::size_type iIdent = strContent.find('=');
		tmp.strKey = strContent.substr(0, iIdent);
		tmp.strContent = strContent.substr(++iIdent, string::npos);
		vecstr.push_back(tmp);
		return true;
	}
	while (string::npos != isplit)
	{
		string strtmp = sContent.substr(0, isplit++);
		string::size_type iIdent = strtmp.find('=');
		if (string::npos != iIdent)
		{
			tmp.strKey = strtmp.substr(0, iIdent);
			tmp.strContent = strtmp.substr(++iIdent, string::npos);
			vecstr.push_back(tmp);
		}

		sContent = sContent.substr(isplit, string::npos);
		isplit = sContent.find(cSplit);
		if (isplit == string::npos)
		{
			iIdent = sContent.find('=');
			if (string::npos != iIdent)
			{
				tmp.strKey = sContent.substr(0, iIdent);
				tmp.strContent = sContent.substr(++iIdent, string::npos);
				vecstr.push_back(tmp);
			}
		}
	}
	return true;
}
bool CSujectKBOrder::SplitNoticePosDetail(const string &strContent, char cSplit, vector<NoticeContent> &vecstr)
{
	string sContent = strContent;
	string::size_type npos = sContent.find(';');
	if (string::npos != npos)
	{
		sContent = sContent.substr(++npos, string::npos);
		npos = sContent.find(';');
		NoticeContent tmp;
		for (int i = 0; i < 4; i++)
		{
			if (string::npos != npos)
			{
				string strtmp = sContent.substr(0, npos++);
				string::size_type iIdent = strtmp.find('=');
				if (string::npos != iIdent)
				{
					tmp.strKey = strtmp.substr(0, iIdent);
					tmp.strContent = strtmp.substr(++iIdent, string::npos);
					vecstr.push_back(tmp);
				}
				else
					return false;
				sContent = sContent.substr(npos, string::npos);
				npos = sContent.find(';');
			}
			else
				return false;
		}
		string::size_type iIdent = sContent.find('=');
		if (string::npos != iIdent)
		{
			tmp.strKey = sContent.substr(0, iIdent);
			tmp.strContent = sContent.substr(++iIdent, string::npos);
			vecstr.push_back(tmp);
		}
		else
			return false;
	}
	else
		return false;

	return true;
	
}
bool CSujectKBOrder::SplitNoticePosTotal(const string &strContent, char cSplit, vector<NoticeContent> &vecstr)
{
	string sContent = strContent;
	string::size_type npos = sContent.find(';');
	if (string::npos != npos)
	{
		sContent = sContent.substr(++npos, string::npos);
		npos = sContent.find(';');
		NoticeContent tmp;			
		for (int i = 0; i < 3; i++)
		{
			if (string::npos != npos)
			{
				string strtmp = sContent.substr(0, npos++);
				string::size_type iIdent = strtmp.find('=');
				if (string::npos != iIdent)
				{
					tmp.strKey = strtmp.substr(0, iIdent);
					tmp.strContent = strtmp.substr(++iIdent, string::npos);
					vecstr.push_back(tmp);
				}
				else
					return false;
				sContent = sContent.substr(npos, string::npos);
				npos = sContent.find(';');
			}
			else
				return false;
		}
		string::size_type iIdent = sContent.find('=');
		if (string::npos != iIdent)
		{
			tmp.strKey = sContent.substr(0, iIdent);
			tmp.strContent = sContent.substr(++iIdent, string::npos);
			vecstr.push_back(tmp);
		}
		else
			return false;
	}
	else
		return false;

	return true;
}
CSujectKBOrder *g_pSubject = NULL;
CSujectKBOrder *GetOFInputDlg()
{
	if (!g_pSubject)
		g_pSubject = new CSujectKBOrder();
	return g_pSubject;
}

//通知回调
void OnLinkageAction(HWND From, HWND To, const char* Sender, const char* Action, const char* Content)
{
	GetOFInputDlg()->RecNotice(From, To, Sender, Action, Content);
}

void RegHotKey(const bool alt, const bool ctrl, const bool shift, const unsigned char vkey, const bool exclusive)
{
	if (vkey != '\0')
		g_PlugLoad.GetMainFrm()->RegHotKey(alt, ctrl, shift, (unsigned char)vkey, exclusive, (TOnHotKeyDown)OnHotKeyDown);
}
void RegFrmHotKey()
{
	//横向下单快捷键
	RegHotKey(true, false, false, Vk_Condition);//条件
	RegHotKey(true, false, false, Vk_CloseOpen);//平开
	RegHotKey(true, false, false, Vk_Fast);		//快速
	RegHotKey(true, false, false, Vk_BatchTimes);//批次
	RegHotKey(true, false, false, Vk_BatchQty);	//批量
	RegHotKey(true, false, false, Vk_OK);		//确定
	RegHotKey(true, false, false, Vk_PreOrder);	//埋单
}
//注册热键回调
void __cdecl OnHotKeyDown(const bool Alt, const bool Ctrl, const bool Shift, const unsigned char VKey, const bool Exclusive, HWND source)
{
	if (Exclusive)
	{
		if (GetOFInputDlg()->GetAllDlg().size()>1)
		{
			vecAllOFInputDlg::iterator iter = GetOFInputDlg()->GetAllDlg().begin() + (GetOFInputDlg()->GetAllDlg().size()-1);
			(*iter)->OnHotKeyDown(Alt, Ctrl, Shift, VKey, Exclusive, source);
			return;
		}
			
		for (vecAllOFInputDlg::size_type i = 0; i < GetOFInputDlg()->GetAllDlg().size(); i++)
			GetOFInputDlg()->GetAllDlg()[i]->OnHotKeyDown(Alt, Ctrl, Shift, VKey, Exclusive, source);
	}
}

//菜单回调
void __cdecl OnMenuItemClick(const unsigned int MenuIndex, const HWND PastLife)
{
	WinProperty wProperty;

	if (PastLife != 0)
	{
		char cProperty[100] = { 0 };
		g_PlugLoad.GetMainFrm()->GetProperty(PastLife, cProperty, 100);
		g_PlugLoad.GetMainFrm()->SetProperty(PastLife, "");
		vector<NoticeContent> vInfo;
		CConvert::SplitNotice(cProperty, ';','=', vInfo);
		for (vector<NoticeContent>::iterator iter = vInfo.begin(); iter != vInfo.end(); iter++)
		{
			if (!strcmp(iter->strKey.c_str(), FastCheck))
				wProperty.bFastCheck = atoi(iter->strContent.c_str()) ? true : false;
			if (!strcmp(iter->strKey.c_str(), ConditionCheck))
				wProperty.bConditionCheck = atoi(iter->strContent.c_str()) ? true : false;
			if (!strcmp(iter->strKey.c_str(), BackHandCheck))
				wProperty.bBackHandCheck = atoi(iter->strContent.c_str()) ? true : false;
			if (!strcmp(iter->strKey.c_str(), BatchCountCheck))
				wProperty.bBatchCountCheck = atoi(iter->strContent.c_str()) ? true : false;
		}
	}
	if (MenuIndex == g_PlugLoad.m_idmenu)
	{
		COFInputDlg*pdlg = GetOFInputDlg()->CreateObject(wProperty);
		if (pdlg)
		{
			//ime输入法更改为英文状态
			LoadKeyboardLayout(_T("0x0409"), KLF_ACTIVATE | KLF_SETFORPROCESS);
			pdlg->ShowDlg(g_hinst, IDD_FUTUREINPUT_DIALOG, NULL);
			if (pdlg->GetHwnd())
			{
				IMainFrame* tmpMainFrm = g_PlugLoad.GetMainFrm();
				if (tmpMainFrm)
				{
					HWND tmpHwnd = pdlg->GetHwnd();
					tmpMainFrm->PutInnerEx(tmpHwnd, OnLinkageAction);
					SetWindowTextW(tmpHwnd, LoadResString(IDS_STRING_RegMenu).c_str());
				}
			}
			
		}
	}
}
