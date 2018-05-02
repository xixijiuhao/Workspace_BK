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
    void FindItem(const void* data, int& iRow)
    {
		uint i = 0;
        for (; i < m_VecData.size();i++)
        {
			if (m_VecData[i] == data)
				break;
        }
		iRow = i;
    }
	void Delete(uint i)//ÀàËÆat
	{
		if (i >= m_VecData.size())
		{
			return;
		}
		else
		{
			m_VecData.erase(m_VecData.begin()+i);
		}
	}
	void erase(const void* pData)
	{
		auto iter = find(m_VecData.begin(),m_VecData.end(),pData);
		if (iter != m_VecData.end())
			m_VecData.erase(iter);
	}
public:
	VecVoidData m_VecData;
}; 

void GetFilePath(TCHAR* filePath, int length, TCHAR* fileName);
void SplitString(TCHAR *strSource, TCHAR split, vector<wstring>& vecContent);

LRESULT OnCtrlColor(WPARAM wParam, LPARAM lParam);
#endif