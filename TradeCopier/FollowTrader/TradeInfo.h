#ifndef _TRADEINFO_H_
#define _TRADEINFO_H_

class TradeInfo :public TIxFrm
{
public:
	TradeInfo();
	~TradeInfo();
	void Create(HWND hParent);
	void MoveWindow(int x, int y, int cx, int cy);
public:
	void PushBackOrder(const TOrder*pOrder);
	void OnPosition();
	void SetUser(vector<TUser>* pMain, vector<TFollowUser>* pFollow);
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnCreate();
	void OnPaint();
	void OnTimer();
	void OnSize(int cx, int cy);
	void OnNotify(WPARAM wParam, LPARAM lParam);
	void OnDrawItem(WPARAM wParam, LPARAM lParam);
	void OnMeasureItem(WPARAM wParam, LPARAM lParam);
	void DrawTabBtn(HDC hdc);
	void OnLButtonDown(LPARAM lParam);
	void OnClickBtn(WPARAM wParam, LPARAM lParam);
private:
	void OnLVNotify(TListView *pList, LPNMITEMACTIVATE p);
	void ShowHide();
	void GetPosition();
	void UpdatePosition();
	void UpdatePosProfit();
	void AddProfitItem(int iRow, int iColm, double data);
	void Cover(int index, double coverPercent =1);
	void UpdateOrderState(const TOrder* pOrder, TListView *listView, int iRow);
private:
	void SetBtnPos();
	void CoverAll();
	bool CheckUser(const char* pszUserNo);
private:
	void CoverSHFEPos(const TPosition*pPosition);
private: 
	TListView		m_lvMainOrder;
	TListView		m_lvFollowOrder;
	TListView		m_lvPosition;
	TStatic			m_labOrderState;   //ί����Ϣ

	TStaticButton   m_btnCover[4]; //33%��50%��100%��All
private:
	vector<TFollowUser>*	m_pFollowUser; //�����˺�
	vector<TUser>*			m_pMainUser;		  //���˺�
	list<const void*>		m_mainOrder;    //���˺�ί��
	list<const void*>		m_followOrder;  //�����˺�ί��
	vector<PositionInfo>    m_totalPosition;
	vector<RECT>            m_vecTabBtnRc;
	int						m_iNowSelTab;
	int						m_iLvHeight;
	bool					m_bAfterPosition;
	enum
	{ 
		id_lvMainOrder = 1,
		id_lvFollowOrder,
		id_lvPosition,

		id_btnCover33,
		id_btnCover50,
		id_btnCover100,
		id_btnCoverAll,
	};
};

#endif