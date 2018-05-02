#ifndef _T_RISK_CONFIG_FRM_H
#define _T_RISK_CONFIG_FRM_H

class TLockWindow;
class TDealRisk;
class TRiskConfigFrm :public TIxFrm
{
public:
	TRiskConfigFrm();
	~TRiskConfigFrm();
	static HWND __stdcall  create_frame(const wchar_t * sub_title, const HWND parent);
protected:
	LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);
	void GetData(Rc_ui_input_para &data);
	void SetData(Rc_ui_input_para &data);
	void Enable(bool bEnable,int index);
	void EnableSpinEdit(int index);
	void Init();
	void SaveData(Rc_ui_input_para &data);
	void ReadData(Rc_ui_input_para &data);
	RC_ACTION GetCbSelect(UINT index);
	int GetSelectIndex(UINT iCombox);
	void SplitString(char*szString, vector<int>&data);
	void ShowRiskState(); //显示风控状态，，运行，停止
private:
	void OnPaint();
	void OnCreate();
	void OnDestroy();
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnCbSelect(WPARAM wParm, LPARAM lParam);
	void OnClickBtn(WPARAM wParm, LPARAM lParam);
private:
	void DrawHeader(TMemDC *pMemDC);
	void DrawText(TMemDC *pMemDC);
	void DrawGrid(TMemDC *pMemDC);
private:
	TCheckBox         m_ckAction[5];
	TEdit             m_editParam[5];
	TCombox           m_cbCloseTime[5];  //暂停多久
	TPinEdit          m_peditMinutes[5];
	TStaticButton     m_btnExecute[3];   //加载默认数据，取消，应用
	TStatic           m_riskState;       //风控状态
private:
	Rc_ui_input_para  m_stDefaultData;   //默认数据
	Rc_ui_input_para  m_stRiskData;
	char m_szCbText[4][10]; //一刻，半天，全天，任意
	HBRUSH m_hbrStatic;
	enum{id_btnApply=0,id_btnDefault,id_cancelRisk};
	enum{filed_ck=0,filed_cb,filed_spin,filed_clc,filed_sclc,filed_mlp,filed_mslp,filed_md};
};

class TLockWindow:public TModalWnd
{
public:
	TLockWindow(HWND hwnd,char* szTitle,int lockSecond);
	~TLockWindow();
	void SetInfo(RC_RULE rcRule, Rc_ui_input_para &data);
protected:
	void CreateSubCtrl();
	void SetModalRect();
	void DrawMain(TMemDC* pmemdc);
	LRESULT	WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnCreate();
	void OnTimer();
private:
	void SetTimeText();
private:
	int   m_iLockSecond;
	char  m_lastTime[9];
	char  m_strInfo[80];  //提示触发了哪个条件
	Rc_ui_input_para m_stRiskData;
	RC_RULE m_rcRule;
};

class TDealRisk :public TIxFrm
{
public:
	void Run();
	~TDealRisk();
	static TDealRisk *Instance();
	void SetData(Rc_ui_input_para &data);
	static void __cdecl OnReceiveNotice(const char* sender, const char* action, const char* content);
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	int DealRisk(RC_RULE rcRule, Rc_ui_input_para &data);
	void OnRiskControl();
private:
	static TDealRisk* pDealRisk;
	static string     strNoticeContent;
	Rc_ui_input_para  m_stRiskParam;
};
#endif