#ifndef STOPLOSS_PARAMFRM_H
#define STOPLOSS_PARAMFRM_H

class TStopLossParamFrm :public TIxFrm, public IOperator
{
public:
	TStopLossParamFrm();
	~TStopLossParamFrm();
	void ShowFrm(HWND hParent, RECT rect,UINT show);
	void MoveWindow(const int &x, const int &y, const int &cx, const int &cy);
	void RegistSt(stStopLossConfig* pstConfig){ if (pstConfig == NULL)return; m_pstConfig = pstConfig; };
	void UpdateConfig();

protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnPaint();
	void OnCreate();
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnNotify(WPARAM wParam, LPARAM lParam);
	void DrawTextMessage(TMemDC* pmemdc);
	void OnBtnClick(WPARAM wParam, LPARAM lParam);
	void OnSearchCode();
	void OnBtnAdd();
	void OnBtnDelete();
	void OnMouseWheel(WPARAM wParam, LPARAM lParam);
	void ShowList();
	void OnMeasureListView(LPARAM lParam);
	void OnDrawListView(WPARAM wParam, LPARAM lParam);
public:
	virtual void SetContractNo(string sCode);
	virtual void SetPriceType(wstring str){};
	bool GetContract(TContractKey& ContractKey);

private:
	void InsertMap();
	void DeleteMap();
	bool FindItem(int index, stSLContractPriceMargin& st);
	void SetContent(stSLContractPriceMargin st);
	void GetContractKey(char* cContractKey, TContractSingleKey stContract);
	void GetPriKey(char* cContractKey, stSLContractPriceMargin stQty);
	bool GetMinChangePrice(TContractKey& key);
private:
	stStopLossConfig*	m_pstConfig;

	CxCombox			m_cbContinuity;
	CxSpinEdit			m_peditTriggerCount;
	CxCombox			m_cbOpenAutoStopLossBasicPrice;
	CxCombox			m_cbDefaultTactic;
	CxEdit				m_editContractNo;
	CxEdit				m_editMinChangePrice;
	CxSpinEdit			m_spinStopLoss;
	CxSpinEdit			m_spinStopProfit;
	TStaticButton		m_btnAdd;
	TStaticButton		m_btnDelete;
	TStaticButton		m_btnSearch;
	CPopWinCode		    m_PopupContractCode;
	CPopWinCodeList		m_PopupCodeList;
	TListView			m_listStopLoss;
	CxCheck				m_ckOpenAutoStopLoss;
	CxCombox			m_cbStopLossOrderPrice;
	CxCombox			m_cbStopProfitOrderPrice;

private:
	TDataContainer  m_clUserContainer;
	char            m_UserNo[21];
	char            m_Sign1[21];
	char            m_Sign2[21];
	std::string     m_sCode;
	bool            m_bshowCodeList;
	int             m_PriSize;
public:
	//常量
	//三块区域左上角坐标
	const int FIRST_COL_X_OFFSET = 0;
	const int FIRST_COL_Y_OFFSET =  -15;
	const int SECOND_COL_X_OFFSET = 300;
	const int SECOND_COL_Y_OFFSET = 10;
	const int THIRD_COL_X_OFFSET = 550;
	const int THIRD_COL_Y_OFFSET = 0;
	enum
	{
		ID_EDITCONTRACTNO = 0,
		ID_BTNSEARCH,
		ID_BTNADD,
		ID_BTNDELETE,
		ID_LISTDEFAULTQTY
	};
};


#endif