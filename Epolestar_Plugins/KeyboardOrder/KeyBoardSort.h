#pragma once

enum KeyBoardSortControlID
{
	KS_START,

	KS_ListBox,
	KS_Default_Button,
	KS_Up_Button,
	KS_Down_Button,
	KS_Ok_Button,
	KS_Close_Button,

	KS_END
};
class KeyBoardSort :public TIxFrm
{
public:
	KeyBoardSort();
	virtual ~KeyBoardSort();
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	void Regist(IKeyOrderCfg* p);
	void UnRegist(IKeyOrderCfg* p);
private:
	set<IKeyOrderCfg*>		m_vNotify;										//�����仯֪ͨ��������
	C_KeyBoardOrderCfg		m_KeyBoardCfg;

	void OnCreateWindow();					
	void OnCommond(WPARAM wParam, LPARAM lParam);					
	void OnPaint(WPARAM wParam, LPARAM lParam);			
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
private:
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);	

	HWND					m_hControl[KS_END - KS_START + 1];				//�ؼ��������
	HBRUSH					m_hStaticBKBrush;								//��̬�ı�������ˢ
private:	
	void get_param_file(char * buf, size_t len);
	bool LoadParam(bool bUpdateToUI = false);								//�����ļ����� �õ�����
	bool SaveParam(vector<C_OrderField>&TempArray);							//����������ļ�
private:
	int						m_iOrderFieldCount;								//�ֶνṹ����	
	C_OrderField			*m_pOrderField;									//�ֶνṹָ��

	void GetOrderSortCfg(vector<C_OrderField>&TempArray, vector<C_OrderField>&arrtempofPrice, vector<C_OrderField> &arrtempofTriggerPrice, vector<C_OrderField> &arrtempofIncrePrice, C_OrderField&tempof, C_OrderField &tempSeleCode);
	void LoadDefaultFieldConfig(vector<C_OrderField> &InputCfgArray);
	C_OrderField CreateOF(int id, int width = 100, int height = 30, int type = TFT_STRING, int align = 1, BOOL bShow = TRUE);
	void MoveItemByString(wstring str, int index);
	wstring GetSortWinText(int nID);
private:
	CxKeyBoardButton m_ButtonDefault;
	CxKeyBoardButton m_ButtonUp;
	CxKeyBoardButton m_ButtonDown;
	CxKeyBoardButton m_ButtonOk;
	CxKeyBoardButton m_ButtonClose;
};