#ifndef _TFORM_PROCESS_H
#define _TFORM_PROCESS_H

/*****************************************************/
//discription 表单的信息展示用于"委托流程信息"
//
/*****************************************************/
class TFormProcess : public TDataInfo
{
public:
	TFormProcess();
	~TFormProcess();
public:
	virtual		bool			Create(HWND hparent, LPCTSTR pstr = NULL, int nCol = 0);
	void						SetData(THisOrderProcContainer* pdata);
	virtual		void			RefreshFilterData();
protected:
	void						OnCommand(WPARAM wParam, LPARAM lParam);
	void						OnGridCtrlAction(WPARAM wParam, LPARAM lParam);
	virtual		LRESULT			WndProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:
	virtual		void			UpdateList(int nstartrow);
	virtual		void			UpdateListText(int nrealindex, TOrder* porder, GV_ITEM& item);
	virtual		void			SetAutoWidth();
	virtual		void			ChangeColConfig();
protected:
	TVirtualContainer*			m_pcontainer;
protected:
	void						GetLocaldata();
	virtual		void			LoadConfig();
};
#endif