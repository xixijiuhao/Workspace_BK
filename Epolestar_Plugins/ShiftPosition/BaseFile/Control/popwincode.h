#ifndef _POP_WIN_CODE_
#define _POP_WIN_CODE_

#define N_CODE_WIDTH	80
#define N_CODE_HEIGHT	40
#define ID_TREE_VIEW    100
#define ID_LIST_VIEW    101

#define LINE_VALID		15

class CPopWinCode :public TIxFrm
{
    struct CommodityAndKey
    {
        SCommodity *pCommodity;
        char sKey[20];

        CommodityAndKey(SCommodity *commodity) : pCommodity(commodity)
        {
            UpdateKey();           
        }

        void UpdateKey();
    };
    class Commodity_Compare
    {
    public:
        bool operator() (const CommodityAndKey *p1, const CommodityAndKey *p2)
        {
            return strcmp(p1->sKey, p2->sKey) < 0;
        }
    };
	//class Less_Commodity
	//{
	//public:
	//	Less_Commodity()
	//	{

	//	}
	//	void GetKey(const SCommodity* pCommodity,std::string &strKey);
	//	bool operator ()(const void *p1, const void *p2)
	//	{
	//		std::string strKey1, strKey2;
	//		GetKey((const SCommodity*)p1, strKey1);
	//		GetKey((const SCommodity*)p2, strKey2);

	//		return strKey1 < strKey2;
	//	}
	//};
public:
	CPopWinCode();
	~CPopWinCode();

	void	CreatePopWin(IOperator*opr, LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0, int nleft = 0, int ntop = 0, UINT nWidth = 432, UINT nrow = 10);	//������������
	
	bool	UpdateData(char * cSign);

	bool	GetCurOption(std::string & str);

	void	SelectCode();
	//////////////////////////////////////////////////////////////////////////
	bool	IsOptionType(char CommodityType);

	bool	IsOpthinCmbType(char CommodityType);

	bool	IsSpreadType(char CommodityType);

	bool	IsLMEFutures(const char *szExchange, const char *szSign);

	//void	GetContractByOption(TDataContainer & ConDataCon, std::set<std::string> &setContract);
	//////////////////////////////////////////////////////////////////////////
	void	UpdateListCtrl(uint nrow=0);										//���º�Լ�б�

	void	ClearListCtrl();										//����б����� 

	void	TravelItem(HTREEITEM hItem);							//����ɾ��

	char*	GetParam(const char *szInput, char CommodityType);		//��ȡ�����

	void	DelLParamPtr();//�ͷ��ڴ�								//ɾ������

	int		GetTreeParam(HTREEITEM hItem);							//�õ��� ������Ϣ

	bool	GetItemByH(HTREEITEM hItem, TVITEM & item);				//
	
	void	UpdateContract(int Type);								//���º�Լ���뵽��
	
	int		m_sel_row;												//�б���� ����ƶ�������Ϣ
	int		m_sel_col;

	WNDPROC m_ListViewProc;//
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	
	COLORREF m_ColorFG;		//����ǰ��ɫ
	COLORREF m_ColorBK;		//���ڱ���ɫ
	RECT	 m_reClient;	//���ھ��ο�

	UINT     m_nRow;		//����
	int		 m_nUintx;		//��Ԫ���
	int		 m_nUinty;		//��Ԫ�߶�
	int      m_nLeft;		//�����x����
	int      m_nTop;		//�����y����
	int		 m_nWidth;		//���ڿ��
	int		 m_nHeight;		//���ڸ߶�

	HWND     m_hTreeView;	//���ؼ����
	HWND     m_hListView;	//�б�ؼ����
	
	bool     m_bIniCompleted; //��������ʶδ�ı�
	std::string						m_sCode;			//��Լ����
	std::string						m_sExCom;			//��Լ����
	std::string						m_strSign;			//ѡ��������sign
	std::string						m_strCurExchange;	//ѡ��Ľ�����
	std::string						m_strCurCommodity;	//ѡ���Ʒ��
	char						m_CurVarietyType;	//Ʒ������
	std::string						m_strCurContract;	//��Ȩʱ��ǰ�ĺ�Լ 401  405 ��
	std::vector<void*>				m_AllContract;		//����Լ���� ����תΪTContract ����תΪDC_ContractDetail
	std::vector<std::string>				m_AllOptionContract;//�ؼ���ִ�м� 

	IOperator* m_opr;		//����ָ��	

	//bool m_Show_LME_Contract;//LME��ʾ�����Լ����
};
#endif