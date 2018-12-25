#pragma once

#include "wintools.h"

class TIUpgrader;

class CUpgrader
{
private:
	TIUpgrader* m_pIUpgrader;
	HINSTANCE m_hDll;
//单例模式begin
public:
	static CUpgrader * GetInstance(char * ConfigFileName = NULL);
	static TCriticalSection m_CritSec;									//互斥变量, 锁定作用域(变作用域为临界区)
protected:
	CUpgrader(char * ConfigFileName);
	CUpgrader(const CUpgrader&);
	CUpgrader& operator= (const CUpgrader&);
//单例模式end
public:
	~CUpgrader();
	void SetProxy(char * ProxyIp, int ProxyPort);
	void Upgrader(int AutoUpgrade=0);
};