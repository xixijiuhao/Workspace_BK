#ifndef _T_WRITEINFO_H_
#define _T_WRITEINFO_H_

class TWriteInfo
{
public:
	TWriteInfo();
	~TWriteInfo();
public:
	void WriteTradeData(int which,IDataContainer *pContainer, HWND hwnd = NULL);
private:
	void WritePositonInfo(LPCWSTR pszFilePath, IDataContainer *pContainer, HWND hwnd = NULL);
	void WriteOrderInfo(LPCWSTR pszFilePath, IDataContainer *pContainer, HWND hwnd = NULL);
	void WriteMathchInfo(LPCWSTR pszFilePath, IDataContainer *pContainer, HWND hwnd = NULL);
private:
	void OrderState2Char(TOrderState& tstate, TStrategyType& ttype, char* sztext, int nsize);
public:
	enum
	{
		writePositionData,
		writeOrderData,
		writeMatchData,
	};
};

#endif