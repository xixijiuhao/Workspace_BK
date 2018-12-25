#pragma once


class TShiftCondition :public VirtualWnd
{
public:
	TShiftCondition();
public:
	void Regist(std::map<string, ShiftListInfo>* pmapListInfo, std::map<int, ShiftCondition>* pmapshiftCondition, std::map<string, TUserInfo*>* pstUserInfo);
	void ShowCondition(const char* UserNo, const char* UserNo2, const char* ShiftContractNo);
	void ModifyCondition(int OrderId);
protected:
	virtual void OnCreate();  //创建页面控件
	virtual LRESULT OnNotify(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnDealMsg(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnBtnClick(WPARAM wParam, LPARAM lParam);
	virtual void DrawMain(TMemDC& pmemdc);
private:
	void CreateWidget();
	void InitWidget();
	void InitWidgetData(bool IsModify =false);
	void SetWidgetPos();
	void OnBtnOk();
	bool CheckOrderPrice(double OrderPrice, ShiftListInfo shiftInfo);
	void Show();
	//double GetMiniPrice(char* ShiftContracNo, int& iPresicion);
	
	TStatic     m_labUserNo;
	CxCombox    m_cbUserNo;
	TStatic		m_labUserNo2;
	CxCombox	m_cbUserNo2;

	TStatic		m_labContractNo;
	CxEdit		m_editContractNo;

	TStatic		m_labCondition;
	CxCombox	m_cbPrice;
	CxCombox	m_cbCondition;
	CxSpinEdit  m_sEditTriggerPrice;
	TStatic     m_labDirect;
	CxCombox    m_cbDirect;
	TStatic     m_labFirstPrice;
	CxCombox    m_cbFirstPrice;
	TStatic     m_labSecondPrice;
	CxCombox    m_cbSecondPrice;
	CxCombox	m_cbStrategy;

	TStatic		m_labOpenQty;
	CxSpinEdit	m_sEditOpenQty;

	TStaticButton m_btnOk;
	TStaticButton m_btnCancel;
	enum 
	{
		ID_LABUSERNO = 0, ID_CBUSERNO,ID_LABUSERNO2,ID_CBUSERNO2,
		ID_LABCONTRACTNO, ID_EDITCONTRACTNO,
		ID_LABCONDITON, ID_CBPRICE, ID_CBCONDITION, ID_EDITTRIGGERPRICE, ID_SPINTRIGGERPRICE, 
		ID_LABDIRECT, ID_CBDIRECT,
		ID_LABFIRSTPRICE, ID_CBFIRSTPRICE, ID_LABSECONDPRICE, ID_CBSECONDPRICE,
		ID_BTNOK, ID_BTNCANCEL,ID_STRATEGY,
		//2017-01-17
		ID_LAB_OPENQTY, ID_EDIT_OPENQTY, ID_SPIN_OPENQTY
	};
	
private:
	int  m_iOrderId;//条件单Orderid
	char m_cshiftContractNo[51];
	char m_UserNo[21];
	char m_UserNo2[21];
	int  m_iWndType;
	std::map<std::string, TUserInfo*>*	m_pstUserInfo;
	std::map<string, ShiftListInfo>*	m_pmapShiftListInfo;
	std::map<int, ShiftCondition>*		m_pmapShiftCondition;

private:
	int m_giOrderId;

private:
	
};