#ifndef _T_THREEKEYDATA_H_
#define _T_THREEKEYDATA_H_

class TThreeKeyData
{
public:
	TThreeKeyData();
	~TThreeKeyData();
	void SetCurUser(int index);
	int OnUserLogin(bool bNeedSetUser = false);
	vector<string>* GetComboxContent(){ return &m_wstrUserNo; }
	const TMoney* GetUserMoney();
	
	int IsUserChange(string &strUserNo);
	void* GetCurrencyNo(){ return &m_mapUserCurrencyNo; }

	bool CanUpdateMoney(const TMoney *pMoney);

	int GetLoginUserCount(){ return m_wstrUserNo.size(); }
private:
	void ReadCurrency();
	bool IsForeign(const char* pLoginClassName);
private:
	vector<string> m_wstrUserNo; //保存UserNo以便放入combox中
	map<string, vector<string>> m_mapUserCurrencyNo;

	string  m_strCurrencyNo;
};

#endif