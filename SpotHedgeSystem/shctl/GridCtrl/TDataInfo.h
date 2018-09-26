#ifndef _TDATA_INFO_H
#define _TDATA_INFO_H

#define TDeleteMem(pwnd)		\
	if(pwnd){		\
	delete pwnd;	\
	pwnd = NULL;		\
	}

class TDataInfo : public TIxFrm
{
public:
	TDataInfo();
	virtual ~TDataInfo();
public:
	void						SetFontSize(Font_Size fsize);
	Font_Size					GetFontSize() { return m_emfont; }
	virtual		bool			Create(HWND hparent, LPCTSTR pstr = NULL, int nCol = 0);
	void						MoveWindow(int nleft, int ntop, int nright, int nbottom);
	virtual		void			SetAutoWidth(){};
	virtual		void			RefreshFilterData(){}
	virtual		void			Resize(HWND hwnd = NULL);
	virtual		void			ModifyCurrentUser(bool bgetdata = false){};
	//void						SetExpandScroll(TScrollBar* pscrollbar){ m_pscrollbar = pscrollbar; }
	void						GetFilterInfo(string& szinfo);
	int							GetValidAccount();
	virtual		void			ClearFilter() {}
	virtual		void			SetDefSort(string szdefsort) {}
public://改用外部滚动条
	void						_GetHScrollInfo(LPSCROLLINFO stinfo);
	void						_SetHScrollInfo(LPSCROLLINFO stinfo);
protected:
	void						OnPaint();
	void						OnSize();
	void						OnNotify(WPARAM wParam, LPARAM lParam);
	virtual			void		OnShowMenu(WPARAM wParam, LPARAM lParam);
	void						OnGridCtrlColWidth(WPARAM wParam, LPARAM lParam);
	void						OnCommand(WPARAM wParam, LPARAM lParam);
	void						OnGridCtrlSwapCol(WPARAM wParam, LPARAM lParam);
protected:
	void						SwapCol(int nsrc, int ndes);
	void						AutoWidthServer();
	bool						SaveAs(LPCTSTR ptitle, std::wstring& szfilename, wchar_t* pfileformat = NULL, HWND hwnd = NULL);
	bool						IsTraderMenu();
	bool						IsUserTrays();//内盘用户
	bool						IsDaystarUser();//是否启明星后台
	bool						IsUserExternal();//外盘用户
	bool						IsUserExternalSingle();//外盘单客户
	bool						IsUSerExternalTrader();//外盘交易员
	bool						IsExternal_9_0();//仅有外盘9.0
	BOOL						IsOnlyExternal();//仅有外盘用户
	bool						IsDipperUser();//是否包含北斗
	bool						IsTraderHaveRight(TradeUserRight emright);
	bool						IsHaveRight(TradeUserRight* pright, int nsize, TradeUserRight emright);
	BOOL						IsLoginning(TUserInfo* puser);
	//void						ConstructFilter(VFilter* vfilter, string& szinfo);
	void						NotifyFilterChange() { PostMessage(GetParent(), SSWM_FILTER_CHANGE, 0, 0); }
protected:
	virtual		LRESULT			WndProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:
	virtual		void			SetColumnWidth();
	virtual		void			CreateList();
	virtual		void			SetListHeader(){}
	virtual		void			UpdateList(int nstartrow) = 0;
	virtual		void			InitLayout(){}
	virtual		void			ConstructMenu(HMENU hmenu,const int& nlastid){}
	virtual		void			ChangeColConfig(){}
	virtual		void			ConFilterInfo(){}
protected:
	TGridCtrl*					m_pgrid;
protected:
	TLayOut						m_layout;
	HMENU						m_menu;
	std::vector<ConfigData>		m_stlconfig;
	std::string					m_szconfig;
	std::vector<int>			m_stlignore;
	bool						m_bclearfilter;
	string						m_szfilterinfo;
protected:
	_uint						GetListColumn(_uint nrealcol);
	_uint						GetRealColumn(_uint ncol);
	void						GetSpreadId(TMatch* pmatch, char* sztext, int nlength);
protected:
	//TCombox*					CreateCombox(int nwidth, int nheight,bool bedit = false);
	//TEdit*						CreateEdit(int nwidth, int nheight);
protected:
	//TScrollBar*					m_pscrollbar;
	HANDLE						m_handle;
protected://前次data数量
	int							m_npredatacount;
protected://调整选中行
	void						AdjustShowPage(const int ntotalrow,int& nstartrow,bool bautosrcoll);
	void						SaveConfigWidth();
	void						StaticsFormat(wchar_t* ptext, int ntextsize, int nline, int nbuy, int nsell, int nopen, int ncover);
protected:
	bool						m_bfirst;
	BOOL						m_bshowfilter;
public:
	void						ResetFirst(){ m_bfirst = true; }
	void						SetFirst(){ m_bfirst = false; }
	bool						GetFirst(){ return m_bfirst; }
protected:
	Font_Size					m_emfont;
protected:
	LOGFONT						GetCurFont();
};
#endif