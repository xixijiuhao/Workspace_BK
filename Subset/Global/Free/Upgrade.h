#pragma once

#include "wintools.h"

class TIUpgrader;

class CUpgrader
{
private:
	TIUpgrader* m_pIUpgrader;
	HINSTANCE m_hDll;
//����ģʽbegin
public:
	static CUpgrader * GetInstance(char * ConfigFileName = NULL);
	static TCriticalSection m_CritSec;									//�������, ����������(��������Ϊ�ٽ���)
protected:
	CUpgrader(char * ConfigFileName);
	CUpgrader(const CUpgrader&);
	CUpgrader& operator= (const CUpgrader&);
//����ģʽend
public:
	~CUpgrader();
	void SetProxy(char * ProxyIp, int ProxyPort);
	void Upgrader(int AutoUpgrade=0);
};