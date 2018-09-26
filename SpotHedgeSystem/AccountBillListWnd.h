#pragma once

#define TDeleteMem(pwnd)		\
	if(pwnd){					\
	delete pwnd;				\
	pwnd = NULL;				\
	}

#define		AccountCol   19

class AccountBillListWnd :public TIxFrm
{
public:
	AccountBillListWnd();
	virtual ~AccountBillListWnd();

public:
	virtual		bool			Create(HWND hparent, LPCTSTR pstr = NULL, int nCol = 0);
	void						MoveWindow(int nleft, int ntop, int nright, int nbottom);
	virtual		void			Resize(HWND hwnd = NULL);
	void						SetData(TRowInfoContainer* pdata);
protected:
	virtual		void			InitLayout() {}
protected:
	virtual		LRESULT			WndProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:
	void						OnPaint();
	void						OnSize();
	void						OnNotify(WPARAM wParam, LPARAM lParam);
	void						OnCommand(WPARAM wParam, LPARAM lParam);
	void						OnGridCtrlColWidth(WPARAM wParam, LPARAM lParam);
protected:
	virtual		void			UpdateList(int nstartrow);
	virtual		void			UpdateListText(int nrealindex, TRowInfo* pinfo, GV_ITEM& item);
	virtual		void			SetListHeader() {}
	virtual		void			CreateList();
	virtual		void			SetColumnWidth();
private:
	void			SetBaseRowItem(_uint nrow, TRowInfo* pinfo);
	void			SetBuy1RowItem(_uint nrow, TRowInfo* pinfo);
	void			SetBuy2RowItem(_uint nrow, TRowInfo* pinfo);
	void			SetSell1RowItem(_uint nrow, TRowInfo* pinfo);
	void			SetSell2RowItem(_uint nrow, TRowInfo* pinfo);
	void			SetSubTotalRowItem(_uint nrow, TRowInfo* pinfo);
	void			SetSumTotalRowItem(_uint nrow, TRowInfo* pinfo);
protected:
	TGridCtrl*					m_pgrid;
	TRowInfoContainer*			m_pcontainer;
	std::vector<ConfigData>		m_stlconfig0;
	std::vector<ConfigData>		m_stlconfig1;
	//std::vector<ConfigData>		m_stlconfig2;
protected:
	Font_Size					m_emfont;
	LOGFONT						GetCurFont();
protected:
	virtual		void			LoadConfig();
};
