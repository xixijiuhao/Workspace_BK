#pragma once
class Agreement: public TBaseGrid
{
public:
	Agreement();
	virtual ~Agreement();
public:
	virtual		bool			Create(HWND hparent, LPCTSTR pstr = NULL, int nCol = 0);
	void						SetData(AgreementVecType* pdata);
	//virtual		void			RefreshFilterData();
protected:
	void						OnCommand(WPARAM wParam, LPARAM lParam);
	//void						OnGridCtrlAction(WPARAM wParam, LPARAM lParam);
	virtual		LRESULT			WndProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:
	virtual		void			UpdateList(int nstartrow);
	//virtual		void			UpdateListText(int nrealindex, TOrder* porder, GV_ITEM& item);
	//virtual		void			SetAutoWidth();
	//virtual		void			ChangeColConfig();
protected:
	AgreementVecType			* m_pcontainer;
protected:
	void						GetLocaldata();
	//virtual		void			LoadConfig();
};
