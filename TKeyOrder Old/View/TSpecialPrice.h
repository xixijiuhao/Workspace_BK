#pragma  once 
class TSpecialPrice : public VirtualWnd
{
public:
	TSpecialPrice();
	~TSpecialPrice();
public:
	void SetData(const void* pData);
	void SetPositionInfo(const TPosition* pPosition);
	void SetParOrderInfo(const TOrder* pOrder);
	void Create(HWND hParent, uint type = frm_Postion);
protected:
	virtual void OnCreate();
	virtual void OnBtnClick(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnNotify(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnCommand(WPARAM wParam, LPARAM lParam){ return NOT_PROCESSED; };
	virtual LRESULT OnDealMsg(UINT message, WPARAM wParam, LPARAM lParam){ return NOT_PROCESSED; }
	virtual void OnDestroy() { delete this; }

private:
	void OnBtnOk();
	void OnBtnCancle();
	void SetInfo();
private:
	TStatic  m_labContractInfo;
	TStatic  m_labDirectInfo;
	CxSpinEdit m_spinPrice;
	TStaticButton m_btnOK;
	TStaticButton m_btnCancle;
private:
	const TPosition* m_pstPosition;
	const TOrder*    m_pstOrder;
	SOFCommodityInf  m_stCommodityInfo;
	TSendOrder       m_stSendOrder;
	uint			 m_iFrmType;
	enum { ID_BTNOK = 0, ID_BTNCANCLE,ID_SPINPRICE };
public:
	enum{frm_Postion,frm_ParOrder};

};