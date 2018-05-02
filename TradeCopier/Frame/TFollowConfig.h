#ifndef _T_FOLLOW_CONFIG_H_
#define _T_FOLLOW_CONFIG_H_



class TFollowConfig :public TModalWnd
{
public:
	TFollowConfig(HWND hParent, const char* pszTitle);
	~TFollowConfig();
	void MoveWindow(int x, int y, int cx, int cy);
	void SetData(TFollowConfigData *pData){ m_pConfigData = pData; }
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	void SetModalRect();
	void CreateSubCtrl();
	void Init();
	void GetData();
private:
	void OnClickBtn(LPARAM lParam);
private:

	CxCombox			m_cbPriceType;
	CxCombox			m_cbDirect;
	CxEdit				m_editQtyRate; //double
	CxSpinEdit			m_followCoverTime; //追平仓跟单时间
	CxSpinEdit			m_spinChaseCount; //追单次数

	CxSpinEdit			m_spinCheckPositionTime;
	CxCheck				m_ckUseCheckPosition;
	TStaticButton		m_btnApply; //应用
	TStaticButton		m_btnCancel;
	TStatic				m_label[6]; //方向，价格类型，价格比率,追平仓时间,追单次数,检测持仓平衡
	RECT				m_rcPosition;
	TFollowConfigData*  m_pConfigData;


	enum{id_UserNo,
		id_FollowType,
		id_PriceType,
		id_Direct,
		id_QtyRate,
		id_CoverTime,
	    id_btnApply,
		id_btnCancel,
		id_ckUserCheckPosition,
	};
};

#endif