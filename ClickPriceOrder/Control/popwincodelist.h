#ifndef _POP_WIN_CODE_LIST_
#define _POP_WIN_CODE_LIST_

#define N_CODE_LIST_HEIGHT	19
#define WM_CODE_WHEEL_CHANGED	5008
#define WM_CODE_SELECTED		5009
#define ID_LIST_BOX    100

typedef multimap<char, const SContract*>	 MultMap;
struct Result_FuzzyQuery
{
	const SContract*	pContract;			//交易合约信息
	char				szName[256];		//标题
    char                szName2[256];       //标题
    char                sKey[80];           //排序关键字

    void SetContract(const SContract *contract)
    {
        pContract = contract;
        UpdateValue();
    }

    void UpdateValue();
};
class CPopWinCodeList :public TIxFrm
{
	class Less_Commodity
	{
	public:
		Less_Commodity()
		{

		}
		bool operator ()(const Result_FuzzyQuery* p1, const Result_FuzzyQuery* p2)
		{
			return strcmp(p1->sKey, p2->sKey) < 0;
		}
	};
public:
	CPopWinCodeList();
	~CPopWinCodeList();

	void CreatePopWin(IOperator*opr,LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0, int nleft = 0, int ntop = 0, UINT nWidth = 371, UINT nrow = 5);	//创建弹出窗口

	bool UpdateData(const string strSign);	//更新所有交易合约 

	//////////////////////////////////////////////////////////////////////////
    bool IsTypeKeep(char CommodityType);    //是否是需要的品种类型

	bool IsOptionType(char CommodityType);

	bool IsOpthinCmbType(char CommodityType);

	bool IsSpreadType(char CommodityType);

	//void GetContractByOption(TDataContainer & ConDataCon, set<string> &setContract);

	void EditChange(const string str, const string strSign);

	void ClearList();

	string CreateContractTitle(const TContractKey & ContractKey);	//品种开始
	string CreateContractTitle2(const TContractKey & ContractKey);	//交易所开始
	
	void DelListDataPtr();
	void GetAllList(char ch, const string strSign);
	void GetAllExchangeList(char ch, const string strSign);
	void FillListByLen(BOOL bCompareLength = FALSE);
	void FillListByExchange(const string strSign);

	void AddNum();
	void DelNum();
	void OnReturn();
	//////////////////////////////////////////////////////////////////////////
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	COLORREF m_ColorFG;		//窗口前景色
	COLORREF m_ColorBK;		//窗口背景色
	RECT	 m_reClient;	//窗口矩形框

	int      m_nLeft;
	int      m_nTop;
	UINT     m_nRow;
	int		 m_nWidth;		//窗口宽度
	int		 m_nHeight;		//窗口高度

	HWND     m_hListBox;
	string   m_sCode;
	size_t	 m_nLen;
	size_t   m_nLastInputLen;
    string   m_sign;
	
	
	vector<Result_FuzzyQuery*>	m_itemList;
	vector<Result_FuzzyQuery*>	m_itemListByLen;

	
	vector<Result_FuzzyQuery*>	m_itemExchangeList;		//
	vector<Result_FuzzyQuery*>	m_itemExchangeListByLen;//

	IOperator* m_opr;

	map<char, vector<const SContract*>>	 m_VecCom;//以品种名首字母为Key
	map<char, vector<const SContract*>>	 m_VecExc;//以交易所首字母为key
public:
	int m_nCurSel;				//光标索引
};
#endif