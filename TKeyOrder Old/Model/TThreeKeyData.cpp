#include "BaseInclude.h"

TThreeKeyData::TThreeKeyData()
{
	m_wstrUserNo.clear();
}

TThreeKeyData::~TThreeKeyData()
{

}
int TThreeKeyData::IsUserChange(string &strUserNo)
{
	if (strUserNo == g_pGlobalModel->m_strUserNo) return -1;
	TDataContainer stUsers;
	g_pTradeData->GetAllUser(stUsers);
	for (UINT i = 0; i < stUsers.size(); i++)
	{
		const TUserInfo*pUser = (const TUserInfo*)stUsers.at(i);
		if (strcmp(pUser->UserNo, strUserNo.c_str())==0)
		{
			//g_pGlobalModel->m_strUserNo = pUser->UserNo;
			//g_pGlobalModel->m_strSign = pUser->Sign;
			//g_pGlobalModel->m_cUserType = pUser->pLogin->UserType;
			SetCurUser(i);
			return i;
		}
	}
	return -1;
}

int TThreeKeyData::OnUserLogin(bool bNeedSetUser)
{
	TDataContainer stUsers;
	g_pTradeData->GetAllUser(stUsers);
	m_wstrUserNo.clear();
	char cbText[100] = { 0 };
	int iNow = 0;
	int iLoginCount = 0;
	for (UINT i = 0; i < stUsers.size(); i++)
	{
		const TUserInfo*pUser = (const TUserInfo*)stUsers.at(i);
		if (pUser->pLogin&&pUser->pLogin->Logined == bYes && pUser->pLogin->InitProgress==tipReady)
		{
			sprintf_s(cbText, "%s", pUser->UserNo);
			m_wstrUserNo.push_back(cbText);
			if (strcmp(pUser->UserNo, g_pGlobalModel->m_strUserNo.c_str()) == 0)
				iNow = i;
			iLoginCount++;
		}

	}
	ReadCurrency();
	if (iLoginCount == 1 || bNeedSetUser)
	{
		SetCurUser(0);  //设置完用户后要更新资金显示
	}
	return iNow;
}

void TThreeKeyData::ReadCurrency()
{
	m_mapUserCurrencyNo.clear();
	TDataContainer stUser;
	g_pTradeData->GetAllUser(stUser);
	vector<string> vecCurrencyNo;
	for (UINT i = 0; i < stUser.size(); i++)
	{
		const TUserInfo*pUser = (const TUserInfo*)stUser.at(i);
		m_mapUserCurrencyNo.insert(make_pair(pUser->UserNo, vecCurrencyNo));
	}

	TDataContainer stMoney;
	g_pTradeData->GetAllMoney(stMoney);
	for (UINT i = 0; i < stMoney.size(); i++)
	{
		const TMoney* pMoeny = (const TMoney*)stMoney.at(i);
	//	vecCurrencyNo.push_back(pMoeny->CurrencyNo);
		auto iter = m_mapUserCurrencyNo.find(pMoeny->UserNo);
		if (iter != m_mapUserCurrencyNo.end())
		{
			iter->second.push_back(pMoeny->CurrencyNo); 
		}
	}
}

void TThreeKeyData::SetCurUser(int index)
{
	TDataContainer stUser;
	g_pTradeData->GetAllUser(stUser);
	const TUserInfo*pUser = nullptr;
	int iLoginIndex = 0;
	for (int i = 0; i < stUser.size(); i++)
	{
		const TUserInfo*pUserTemp = (const TUserInfo*)stUser.at(i);
		if (pUserTemp->pLogin && pUserTemp->pLogin->Logined == bYes)
		{
			if (iLoginIndex == index)
			{
				pUser = pUserTemp;
				break;
			}
			iLoginIndex++;
		}
	}


	if (pUser)
	{
		g_pGlobalModel->m_strUserNo = pUser->UserNo;
		g_pGlobalModel->m_strSign = pUser->Sign;
		g_pGlobalModel->m_cUserType = pUser->pLogin->UserType;

		g_pGlobalModel->m_utUserType = utInner;
		if (IsForeign(pUser->pLogin->LoginClassName))
			g_pGlobalModel->m_utUserType = utForeign;

		auto iter = m_mapUserCurrencyNo.find(pUser->UserNo);
		if (iter != m_mapUserCurrencyNo.end())
		{
			if (iter->second.size() == 1)
			{
				m_strCurrencyNo = iter->second[0];
			}
			else if (iter->second.size() > 1)
			{
				auto iterVec = find(iter->second.begin(), iter->second.end(), "Base");
				if (iterVec != iter->second.end())
					m_strCurrencyNo = "Base";
				else
					m_strCurrencyNo = iter->second[0];
			}
		}
	}
}

const TMoney * TThreeKeyData::GetUserMoney()
{
	TDataContainer stMoney;
	g_pTradeData->GetAllMoney(stMoney);
	for (UINT i = 0; i < stMoney.size(); i++)
	{
		const TMoney* pMoney = (const TMoney*)stMoney.at(i);
		if (strcmp(g_pGlobalModel->m_strUserNo.c_str(), pMoney->UserNo) == 0 && strcmp(m_strCurrencyNo.c_str(),pMoney->CurrencyNo)==0)
		{
			return pMoney;
		}
	}
	return NULL;
}

bool TThreeKeyData::CanUpdateMoney(const TMoney *pMoney)
{
	return pMoney->CurrencyNo == m_strCurrencyNo &&strcmp(g_pGlobalModel->m_strUserNo.c_str(), pMoney->UserNo) == 0;
}

bool TThreeKeyData::IsForeign(const char* pLoginClassName)
{
	return !strcmp(pLoginClassName, UnicodeToANSI(PLUG_FOREIGNTRADEAPI_NAME).c_str()) || !strcmp(pLoginClassName, UnicodeToANSI(PLUG_DIPPERTRADEAPI_NAME).c_str());
}