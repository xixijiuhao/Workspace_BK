#pragma once

#define sVersion				1

extern vPosDifCfg	vecTmp;				//配置容器

typedef wchar_t							TPosdifListEditStr[51];
typedef wchar_t							PosdifIntW[8];

enum PositionDifConfig
{
	PD_START,
	
	PD_Config_Groupbox,		//组合框

	PD_Code_Edit,
	PD_Code_Button,
	PD_Max1_Edit,
	PD_Max1_Spin,
	PD_Max2_Edit,
	PD_Max2_Spin,
	PD_Hands_Static,

	PD_ListView,
	PD_Add_Button,
	PD_Delete_Button,

	PD_END
};

//仓差配置
class CPositonDiffWin :public TIxFrm ,public IOperator
{
public:
	CPositonDiffWin ();
	virtual ~CPositonDiffWin ();
	void Init();
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	virtual void SetContractNo(string sCode, SCurrency* sCurrency = NULL);
	map<int, WNDPROC>		m_SetEdit;					//子类化窗口过程
	
private:
	void OnCreateWindow();								//窗口创建消息响应

	void OnPaint(WPARAM wParam, LPARAM lParam);		//重绘

	void OnNotify(WPARAM wParam, LPARAM lParam);

	void OnCommond(WPARAM wParam, LPARAM lParam);

	void OnMouseWheel(WPARAM wParam, LPARAM lParam);

	void OnVScroll(WPARAM wParam, LPARAM lParam);

	void OnNMClick(int nID, LPARAM lParam);			//双击回填信息

	void OnSpinDeltaUpDownNormal(int nDelta, int nID, int nMax, int nMin = 0); //数量增减处理

	void OnItemHover(int nID);

	void OnItemLeave(int nID);

	void OnButtonCodeSelect();

	void OnButtonAddItem();

	void OnButtonDeleteItem();

	void GetListViewVec(vPosDifCfg&vec);

	void FillDefaultNumListView(vPosDifCfg&vec);


	//void UpdateDefaultStateCtrl(int nID); //市价

	void		SetWindowInt(HWND hWnd, int nInt);
private:
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);	//窗口处理过程
	void		InitListView();												//初始化listview重绘
private:
	HWND		m_hControl[PD_END - PD_START + 1];	//控件句柄数组
	HBRUSH		m_hStaticBKBrush;					//静态文本背景画刷

	RECT		m_rcRiskTip;		//风险提示区域
	wchar_t		m_sRiskTip[150];	//风险提示信息

	char		m_strPathCodeData[MAX_PATH + 50];	//参数保存文件路径
	wchar_t		m_strPathControl[MAX_PATH + 50];	//参数保存文件路径
	char		m_strPathLog[MAX_PATH + 50];		//参数保存文件路径

//	TLog		m_log;				//设置参数 日志
									
	int			m_StopVer;			//版本号 对应 - TPostionDifInfo

	bool		m_bPosDifDataModify;		//仓差修改


private:
	void		LoadShowInfo();						//加载提示数据 

	bool		LoadParam(bool bUpdateToUI = false); //加载文件数据 得到参数

	bool        SaveParam();		//保存参数到文件

	bool		GetModulePathEx();	//获取文件路径


	CxEdit		m_hwndEdit[3];
	CxSpin		m_hwndSpin[2];
	CxButton	m_hwndBtn[3];
	CPopWinCode m_pPopWinCode;
public:
	WNDPROC					m_ProcListview;

};