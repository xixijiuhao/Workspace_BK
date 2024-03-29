#ifndef _POP_WIN_CODE_
#define _POP_WIN_CODE_


#define N_CODE_WIDTH	80
#define N_CODE_HEIGHT	40
#define ID_TREE_VIEW    100
#define ID_LIST_VIEW    101

#define LINE_VALID		15
using namespace std;
class IOperator
{
public:
	virtual void SetContractNo(string sCode) = 0;
	virtual void SetPriceType(wstring str) = 0;
};
class CPopWinCode :public TIxFrm
{

	class Less_Commodity
	{
	public:
		Less_Commodity()
		{

		}
		void GetKey(const TCommodity* pCommodity, string &strKey);
		bool operator ()(const void *p1, const void *&p2)
		{
			string strKey1, strKey2;
			GetKey((const TCommodity*)p1, strKey1);
			GetKey((const TCommodity*)p2, strKey2);

			bool bcom = strKey1 < strKey2;
			return bcom;
		}
	};
public:
	CPopWinCode();
	~CPopWinCode();
	void	CreatePopWin(IOperator*opr, LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0, int nleft = 0, int ntop = 0, UINT nWidth = 432, UINT nrow = 10);	//创建弹出窗口
	void	CreatePopWin(HWND hWndEdit, LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0, int nleft = 0, int ntop = 0, UINT nWidth = 432, UINT nrow = 10);	//创建弹出窗口

	bool	UpdateData(const char * cSign, bool bLME = false,bool bRemedy = false);

	bool	GetCurOption(string & str);

	void	SelectCode();
	//////////////////////////////////////////////////////////////////////////
	bool	IsOptionType(char CommodityType);

	bool	IsOpthinCmbType(char CommodityType);

	bool	IsSpreadType(char CommodityType);

	bool	IsLMEFutures(const char *szExchange, const char *szSign);

	void	GetContractByOption(TDataContainer & ConDataCon, set<string> &setContract);
	//////////////////////////////////////////////////////////////////////////
	void	UpdateListCtrl(uint nrow=0);										//更新合约列表

	void	ClearListCtrl();										//清楚列表数据 

	void	TravelItem(HTREEITEM hItem);							//遍历删除

	char*	GetParam(const char *szInput, char CommodityType);		//获取查参数

	void	DelLParamPtr();//释放内存								//删除参数

	int		GetTreeParam(HTREEITEM hItem);							//得到树 参数信息

	bool	GetItemByH(HTREEITEM hItem, TVITEM & item);				//
	
	void	UpdateContract(int Type);								//更新合约代码到树
	
	int		m_sel_row;												//列表框内 鼠标移动行列信息
	int		m_sel_col;

	WNDPROC m_ListViewProc;//
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	
	COLORREF m_ColorFG;		//窗口前景色
	COLORREF m_ColorBK;		//窗口背景色
	RECT	 m_reClient;	//窗口矩形框

	UINT     m_nRow;		//行数
	int		 m_nUintx;		//单元宽度
	int		 m_nUinty;		//单元高度
	int      m_nLeft;		//左起点x坐标
	int      m_nTop;		//上起点y坐标
	int		 m_nWidth;		//窗口宽度
	int		 m_nHeight;		//窗口高度

	HWND     m_hTreeView;	//树控件句柄
	HWND     m_hListView;	//列表控件句柄
	
	bool     m_bIniCompleted; //服务器标识未改变
	string						m_sCode;			//合约代码
	string						m_strSign;			//选择交易所的sign
	string						m_strCurExchange;	//选择的交易所
	string						m_strCurCommodity;	//选择的品种
	char						m_CurVarietyType;	//品种类型
	string						m_strCurContract;	//期权时当前的合约 401  405 等
	vector<void*>				m_AllContract;		//按合约排序 交易转为TContract 行情转为DC_ContractDetail
	vector<string>				m_AllOptionContract;//关键字执行价 	

	IOperator* m_opr;		//操作指针	

	bool m_bShowLME;		//

	bool m_bRemedy;

	HWND m_hWndEdit;		//要发送消息的句柄	
};
#endif