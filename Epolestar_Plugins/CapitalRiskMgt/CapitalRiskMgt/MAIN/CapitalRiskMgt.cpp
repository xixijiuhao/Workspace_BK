#include "PreInclude.h"

extern ITradeData *g_pTradeData;
extern ICommonModule *g_pCommonModule;
extern IMainFrame *g_mainframe;
extern IConfigFrame	*g_conFrame;
extern IAuthent *g_authent;
extern IStarApi	*g_StarApi;
extern ICspAccessApi *g_pCspAccessApi;
extern CapitalRiskMgtWnd *g_pCapitalRiskMgtWnd;
extern ITradeApi           *g_pTradeApi;

#define AUTHENT_KEY             "123456"
#define AUTHENT_COMPANY         "EPOLESTAR"
#define EPSILON					1e-6   

HWND __stdcall GetCfgWnd(const wchar_t *subtitle, const HWND parent)
{
	if (wcscmp(subtitle, LoadResWchar_t(IDS_Capital_Risk_Management)) == 0)
	{
		if (g_pCapitalRiskMgtWnd)
		{
			if (!IsWindow(g_pCapitalRiskMgtWnd->GetHwnd()))
			{
				g_pCapitalRiskMgtWnd->CreateCfgWnd(subtitle);
			}
			return g_pCapitalRiskMgtWnd->GetHwnd();
		}
	}
	return NULL;
}

inline double Div(const double &a, const double &b)
{
	if (fabs(b) < EPSILON)
	{
		return 0.0f;
	}
	return a / b;
}

CapitalRiskMgt::CapitalRiskMgt()
{
	wchar_t str11[_MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, str11, sizeof(str11));
	wstring str12 = str11;
	int npos2 = str12.find_last_of(TEXT('\\'));
	str12 = str12.substr(0, npos2);
	swprintf_s(m_sPathSound, L"%s%s", str12.c_str(), L"\\config\\CommonModule\\Fail.wav");
}

void CapitalRiskMgt::OnInitComplete()
{
	if (g_pCapitalRiskMgtWnd)
	{
		g_pCapitalRiskMgtWnd->QryConfigFinish();

		if (g_pCapitalRiskMgtWnd->m_UserNoAndUserInfoMap.size() > 0)
		{
			g_pCommonModule->Regist((ICommonInteractRiskCheck*)this, InteractType_RiskCheck);
			g_pTradeApi->RegistRiskModule((IRiskModuleApi*)this, 0);
			g_conFrame->reg_config_dlg(LoadResWchar_t(IDS_Capital_Risk_Management), (dlg_create_func)GetCfgWnd, cmtAdvance, 15, "esunny.epolestar.configframe");

			auto Iter = g_pCapitalRiskMgtWnd->m_UserNoAndUserInfoMap.begin();
			g_pCapitalRiskMgtWnd->m_sCurUserInGroupNO = Iter->second.GroupNo;
			if (g_pCapitalRiskMgtWnd->m_UserNoAndUserInfoMap.size() == 1)
			{
				g_pCapitalRiskMgtWnd->m_bCurUserCanModify = (Iter->second.OperateRight == '1');
				g_pCapitalRiskMgtWnd->m_MainGroupUserCfg = Iter->second.UserNo;
				return;
			}

			bool isHaveMainUser = false;
			for (; Iter != g_pCapitalRiskMgtWnd->m_UserNoAndUserInfoMap.end(); ++Iter)
			{
				if (Iter->second.OperateRight == '1')
				{
					g_pCapitalRiskMgtWnd->m_bCurUserCanModify = true;
					g_pCapitalRiskMgtWnd->m_MainGroupUserCfg = Iter->second.UserNo;
					break;
				}
			}

			if (!isHaveMainUser)
			{
				g_pCapitalRiskMgtWnd->m_bCurUserCanModify = false;
				g_pCapitalRiskMgtWnd->m_MainGroupUserCfg = g_pCapitalRiskMgtWnd->m_UserNoAndUserInfoMap.begin()->second.UserNo;
			}
		}
	}
}

//【昨 权 益 + 百 分 比】：(昨 权 益 - 今权益) / 昨 权 益 > 百分比，则到达资金回撤
//【固定资金 + 百 分 比】：(固定资金 - 今权益) / 固定资金 > 百分比，则到达资金回撤
//【昨 权 益 + 固定回撤】：(昨 权 益 - 今权益) > 固定回撤，则到达资金回撤
//【固定资金 + 固定回撤】：(固定资金 - 今权益) > 固定回撤，则开始资金回撤

bool CapitalRiskMgt::PositionRangeCheck(const TSendOrder & TSOrder, ErrorReflectInfo & eError)
{
	char cret[100] = { 0 };
	sprintf_s(cret, "m_nCancelStandardType = %d", g_pCapitalRiskMgtWnd->m_cfgData.m_nCancelStandardType);
	OutputDebugStringA(cret);
	sprintf_s(cret, "m_nFixedCancelNum = %f", g_pCapitalRiskMgtWnd->m_cfgData.m_nFixedCancelNum);
	OutputDebugStringA(cret);
	sprintf_s(cret, "m_nCancelMoneyType = %d", g_pCapitalRiskMgtWnd->m_cfgData.m_nCancelMoneyType);
	OutputDebugStringA(cret);
	sprintf_s(cret, "m_nPercentage = %f", g_pCapitalRiskMgtWnd->m_cfgData.m_nPercentage);
	OutputDebugStringA(cret);
	sprintf_s(cret, "m_nFixedMoney = %f", g_pCapitalRiskMgtWnd->m_cfgData.m_nFixedMoney);
	OutputDebugStringA(cret);
	sprintf_s(cret, "m_bIsForbidOn = %d", g_pCapitalRiskMgtWnd->m_cfgData.m_bIsForbidOn);
	OutputDebugStringA(cret);
	sprintf_s(cret, "m_bIsWarningOn = %d", g_pCapitalRiskMgtWnd->m_cfgData.m_bIsWarningOn);
	OutputDebugStringA(cret);
	
	//检查是否设置了资金风控
	bool isPercentageInvial = fabs(g_pCapitalRiskMgtWnd->m_cfgData.m_nPercentage) < EPSILON;
	bool isFixedMoneyInvial = fabs(g_pCapitalRiskMgtWnd->m_cfgData.m_nFixedMoney) < EPSILON;
	bool isHasWarning = g_pCapitalRiskMgtWnd->m_cfgData.m_bIsForbidOn || g_pCapitalRiskMgtWnd->m_cfgData.m_bIsWarningOn;
	if ((isPercentageInvial && isFixedMoneyInvial) || !isHasWarning)
	{
		return true;
	}

	std::string tCode;
	if (!CConvert::TContractKeyToTContractNo(TSOrder, tCode))
	{
		return true;
	}

	if (!CheckMoneyCancelThd(TSOrder, eError))
	{
		if (g_pCapitalRiskMgtWnd->m_cfgData.m_bIsWarningOn)
		{
			PlaySoundW(m_sPathSound, NULL, SND_ASYNC | SND_NODEFAULT);
		}
		
		if (TSOrder.Offset == oOpen && g_pCapitalRiskMgtWnd->m_cfgData.m_bIsForbidOn)
		{
			eError.nErrorLevel = EL_Error;
			wcscpy_s(eError.error_text, LoadResWchar_t(IDS_Forbid_Open_Text));
			return false;
		}
	}
	return true;
}

bool CapitalRiskMgt::CancelPositionRangeCheck(std::set<int>& vecOrderId, char * userNo)
{
	return true;
}

bool CapitalRiskMgt::CheckMoneyCancelThd(const TSendOrder & TSOrder, ErrorReflectInfo & eError)
{
	const TMoney *pMoney = g_pTradeData->GetMoney(TSOrder.UserNo, "", "CNY", TSOrder.Sign);
	if (!pMoney)
	{
		return false;
	}
	if (g_pCapitalRiskMgtWnd->m_cfgData.m_nCancelStandardType == PreEquityEnum)
	{
		//if (g_pCapitalRiskMgtWnd->m_cfgData.m_nCancelMoneyType == PercentageEnum)
		//{
		//	return CheckPercentageThd(pMoney->PreEquity, pMoney->Equity);
		//}
		//else if (g_pCapitalRiskMgtWnd->m_cfgData.m_nCancelMoneyType == FixedCancelEnum)
		//{
		//	return CheckFixedCancelThd(pMoney->PreEquity, pMoney->Equity);
		//}
		return CheckMoneyThd(pMoney->PreEquity, pMoney);
	}
	else if (g_pCapitalRiskMgtWnd->m_cfgData.m_nCancelStandardType == FixedMoneyEnum)
	{
		return CheckMoneyThd(g_pCapitalRiskMgtWnd->m_cfgData.m_nFixedMoney, pMoney);
	}
	return true;
}

bool CapitalRiskMgt::CheckMoneyThd(const double fStandard, const TMoney *pMoney)
{
	if (g_pCapitalRiskMgtWnd->m_cfgData.m_nCancelMoneyType == PercentageEnum)
	{
		return CheckPercentageThd(fStandard, pMoney->Equity);
	}
	else if (g_pCapitalRiskMgtWnd->m_cfgData.m_nCancelMoneyType == FixedCancelEnum)
	{
		return CheckFixedCancelThd(fStandard, pMoney->Equity);
	}
	return true;
}

bool CapitalRiskMgt::CheckPercentageThd(const double firstMoney, const double secondMoney)
{
	double nThreshold = Div((firstMoney - secondMoney), firstMoney);
	if ((nThreshold - g_pCapitalRiskMgtWnd->m_cfgData.m_nPercentage / 100) > EPSILON)
	{
		return false;
	}
	return true;
}

bool CapitalRiskMgt::CheckFixedCancelThd(const double firstMoney, const double secondMoney)
{
	double nThreshold = firstMoney - secondMoney - g_pCapitalRiskMgtWnd->m_cfgData.m_nFixedCancelNum;
	if (nThreshold > EPSILON)
	{
		return false;
	}
	return true;
}
