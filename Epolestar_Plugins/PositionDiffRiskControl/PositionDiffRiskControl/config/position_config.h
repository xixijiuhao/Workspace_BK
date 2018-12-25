#pragma once

#define sVersion				1

extern vPosDifCfg	vecTmp;				//��������

typedef wchar_t							TPosdifListEditStr[51];
typedef wchar_t							PosdifIntW[8];

enum PositionDifConfig
{
	PD_START,
	
	PD_Config_Groupbox,		//��Ͽ�

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

//�ֲ�����
class CPositonDiffWin :public TIxFrm ,public IOperator
{
public:
	CPositonDiffWin ();
	virtual ~CPositonDiffWin ();
	void Init();
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	virtual void SetContractNo(string sCode, SCurrency* sCurrency = NULL);
	map<int, WNDPROC>		m_SetEdit;					//���໯���ڹ���
	
private:
	void OnCreateWindow();								//���ڴ�����Ϣ��Ӧ

	void OnPaint(WPARAM wParam, LPARAM lParam);		//�ػ�

	void OnNotify(WPARAM wParam, LPARAM lParam);

	void OnCommond(WPARAM wParam, LPARAM lParam);

	void OnMouseWheel(WPARAM wParam, LPARAM lParam);

	void OnVScroll(WPARAM wParam, LPARAM lParam);

	void OnNMClick(int nID, LPARAM lParam);			//˫��������Ϣ

	void OnSpinDeltaUpDownNormal(int nDelta, int nID, int nMax, int nMin = 0); //������������

	void OnItemHover(int nID);

	void OnItemLeave(int nID);

	void OnButtonCodeSelect();

	void OnButtonAddItem();

	void OnButtonDeleteItem();

	void GetListViewVec(vPosDifCfg&vec);

	void FillDefaultNumListView(vPosDifCfg&vec);


	//void UpdateDefaultStateCtrl(int nID); //�м�

	void		SetWindowInt(HWND hWnd, int nInt);
private:
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);	//���ڴ������
	void		InitListView();												//��ʼ��listview�ػ�
private:
	HWND		m_hControl[PD_END - PD_START + 1];	//�ؼ��������
	HBRUSH		m_hStaticBKBrush;					//��̬�ı�������ˢ

	RECT		m_rcRiskTip;		//������ʾ����
	wchar_t		m_sRiskTip[150];	//������ʾ��Ϣ

	char		m_strPathCodeData[MAX_PATH + 50];	//���������ļ�·��
	wchar_t		m_strPathControl[MAX_PATH + 50];	//���������ļ�·��
	char		m_strPathLog[MAX_PATH + 50];		//���������ļ�·��

//	TLog		m_log;				//���ò��� ��־
									
	int			m_StopVer;			//�汾�� ��Ӧ - TPostionDifInfo

	bool		m_bPosDifDataModify;		//�ֲ��޸�


private:
	void		LoadShowInfo();						//������ʾ���� 

	bool		LoadParam(bool bUpdateToUI = false); //�����ļ����� �õ�����

	bool        SaveParam();		//����������ļ�

	bool		GetModulePathEx();	//��ȡ�ļ�·��


	CxEdit		m_hwndEdit[3];
	CxSpin		m_hwndSpin[2];
	CxButton	m_hwndBtn[3];
	CPopWinCode m_pPopWinCode;
public:
	WNDPROC					m_ProcListview;

};