#ifndef _T_DATA_CONTAINER_H_
#define _T_DATA_CONTAINER_H_

#define encryptionKey  1364
#define riskConfigFilePath L"\\config\\RiskConfig.pri"
#define pwdConfigAppName   L"PwdConfig"

typedef vector<const void *> VecVoidData;

class TDataContainer :public IDataContainer
{
public:
	virtual uint __cdecl size()
	{
		return m_VecData.size();
	}
	virtual const void * __cdecl at(uint i)
	{
		return m_VecData.at(i);
	}
	virtual void __cdecl clear()
	{
		m_VecData.clear();
	}
	virtual void __cdecl push_back(const void * data)
	{
		m_VecData.push_back(data);
	}
	VecVoidData * GetData()
	{
		return &m_VecData;
	}
public:
	VecVoidData m_VecData;
};

void GetFilePath(TCHAR* filePath, int length, TCHAR* fileName);

#endif