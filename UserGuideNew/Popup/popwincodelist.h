#ifndef _POP_WIN_CODE_LIST_
#define _POP_WIN_CODE_LIST_



#define N_CODE_LIST_HEIGHT	19
#define WM_CODE_WHEEL_CHANGED	5008
#define WM_CODE_SELECTED		5009
#define ID_LIST_BOX    100
using namespace std;

typedef multimap<char, const TContract*>	 MultMap;
struct Result_FuzzyQuery
{
	const TContract*	pContract;			//���׺�Լ��Ϣ
	char				szName[256];		//����
	//COLORREF	rgbColor;			// ��ɫ
};
class CPopWinCodeList :public TIxFrm
{
	class Less_Commodity
	{
	public:
		Less_Commodity()
		{

		}
		void GetKey(const TContract* pContract, string &strKey);
		bool operator ()(const Result_FuzzyQuery* p1, const Result_FuzzyQuery* p2)
		{
			string strKey1, strKey2;
			GetKey(p1->pContract, strKey1);
			GetKey(p2->pContract, strKey2);

			bool bcom = strKey1<strKey2;
			return bcom;
		}
	};
public:
	CPopWinCodeList();
	~CPopWinCodeList();

	void CreatePopWin(IOperator*opr,LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0, int nleft = 0, int ntop = 0, UINT nWidth = 371, UINT nrow = 5);	//������������

	bool UpdateData();	//�������н��׺�Լ 

	//////////////////////////////////////////////////////////////////////////
	bool IsOptionType(char CommodityType);

	bool IsOpthinCmbType(char CommodityType);

	bool IsSpreadType(char CommodityType);

	void GetContractByOption(TDataContainer & ConDataCon, set<string> &setContract);

	void EditChange(const string str, const string strSign);

	void ClearList();
	string CreateContractTitle(const TContractKey & ContractKey);	//Ʒ�ֿ�ʼ
	string CreateContractTitle2(const TContractKey & ContractKey);	//��������ʼ
	
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
	COLORREF m_ColorFG;		//����ǰ��ɫ
	COLORREF m_ColorBK;		//���ڱ���ɫ
	RECT	 m_reClient;	//���ھ��ο�

	int      m_nLeft;
	int      m_nTop;
	UINT     m_nRow;
	int		 m_nWidth;		//���ڿ��
	int		 m_nHeight;		//���ڸ߶�

	HWND     m_hListBox;
	string   m_sCode;
	size_t	 m_nLen;
	size_t   m_nLastInputLen;
	
	
	vector<Result_FuzzyQuery*>	m_itemList;
	vector<Result_FuzzyQuery*>	m_itemListByLen;

	
	vector<Result_FuzzyQuery*>	m_itemExchangeList;		//
	vector<Result_FuzzyQuery*>	m_itemExchangeListByLen;//

	IOperator* m_opr;

	map<char, vector<const TContract*>>	 m_VecCom;//��Ʒ��������ĸΪKey
	map<char, vector<const TContract*>>	 m_VecExc;//�Խ���������ĸΪkey
public:
	int m_nCurSel;				//�������
};
#endif