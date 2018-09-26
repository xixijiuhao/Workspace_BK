#include "PreInclude.h"

extern G_COLOREF			g_ColorRefData;				//色彩
extern G_UIFONT				g_FontData;					//字体
extern ILanguageApi			*g_pLanguage;		//语言资源

Agreement::Agreement()
{
	m_pcontainer = NULL;
}

Agreement::~Agreement()
{
	TDeleteMem(m_pcontainer);
}

bool Agreement::Create(HWND hparent, LPCTSTR pstr, int nCol)
{
	//char szconfig[MAX_PATH];
	//WCharToMByte(pstr, szconfig, MAX_PATH);
	//m_szconfig = szconfig;

	__super::Create(hparent, _T("TAgreement"), 13);

	//m_pcontainer = new AgreementMapType;

	//LoadConfig();
	CreateList();
	SetColumnWidth();
	return true;
}

void Agreement::SetData(AgreementVecType * pdata)
{
	m_pcontainer = pdata;
	int npos = m_pgrid->GetVertPos();
	UpdateList(npos);
}

void Agreement::OnCommand(WPARAM wParam, LPARAM lParam)
{
	__super::OnCommand(wParam, lParam);

	int ntype = HIWORD(wParam);
	if (ntype != 0) return;

	int nmenuid = LOWORD(wParam);
	if (nmenuid == 103)
	{
		int may = 5;
	}
}


LRESULT Agreement::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		break;
	default:
		return __super::WndProc(message, wParam, lParam);
	}
	return PROCESSED;
}

void Agreement::UpdateList(int nstartrow)
{
	if (!m_pgrid) return;
	m_pgrid->SetRowCount(m_pcontainer->size() + 1);

	DataRange strange;
	m_pgrid->GetNonfixDataRange(strange, (nstartrow == 0));
	for (_uint nrow = strange.nbeginrow; nrow <= strange.nendrow; nrow++)
	{
		SmsAgreementDataRsp* porder = NULL;
		porder = &(m_pcontainer->at(nrow - 1));
		if (!porder) break;

		for (_uint ncol = 0; ncol < m_pgrid->GetColumnCount(); ncol++)
		{
			GV_ITEM item;
			memset(&item, 0, sizeof(GV_ITEM));
			item.cellid.ncol = ncol;
			item.cellid.nrow = nrow;
			item.lgfont = GetCurFont()/*g18_font*/;
			if (g_pLanguage->LangId() == ENU) item.nvmargin = 3;
			else item.nvmargin = 5;
			item.nhmargin = 3;
			item.dwmask = GVIF_TEXT | GVIF_FONT | GVIF_FORMAT | GVIF_FGCLR | GVIF_VMARGIN | GVIF_HMARGIN;
			item.dwformat = DT_VCENTER;
			item.dwformat |= DT_CENTER;
			//ConfigData stcofig = m_stlconfig[ncol];
			item.clrfg = g_ColorRefData.GetColorBlack();
			strcpy_s(item.szparam, "ceshi");
			m_pgrid->SetItem(&item);
		}
	}
}

void Agreement::GetLocaldata()
{
}




