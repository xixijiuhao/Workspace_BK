#pragma once

class KLinePresenter : public ISHisQuoteSpi, public TIxFrm
{
public:
	KLinePresenter();
	~KLinePresenter();
	static void OnMenuClick(const unsigned int MenuIndex, const HWND PastLife);

protected:
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	void __stdcall OnHisQuote(const SContractNoType contractno, const SKLineTypeType klinetype, const SSessionIdType sessionid);
	void __stdcall OnHisQuoteTimeLine(const C8[], const SDateType, const SSessionIdType) {};
	//历史波动率通知 仅通知数据结束变化，使用get函数获取数据
	void __stdcall OnHisVolatility(const SContractNoType contractno, const SKLineTypeType klinetype, const SSessionIdType sessionid) {};
public:
	void UpdateKLineView();
	void ShowMainFrm() { m_MainFrm->CreateView(); };
	void ShowSubView(MainFrmRect &rect);
	void ChangeSubViewFrame(MainFrmRect &rect);
	void InitSubViewData(RECT CenterKLineRect) {};
	const SContract* GetContract();
	KContract& GetKContract() { return m_KContract; }
public:
	TMainFrm* getMainFrm();
	KLineModel* getKLineModel();
private:
	KLineModel*			m_KLineModel;				//Model数据类	
	TMainFrm*			m_MainFrm;					//主界面类
	KLineView*			m_KLineView;				//KLine类
	KLineSelectTab*		m_KLineSelectTab;			//选TCTerm类
	KLineIntroductView*	m_KLineIntroduct;			//介绍说明类
	KListView*			m_KListView;				//列表选择类
private:
	void OnTabChange(int index);
	void OnClickListItem(KContract p);
	void GetTCData();
	void GetTCDataFromMap();
	void GetTCDataSpi();
	void UpdateView();
	void LoadFileData();							//依次加载自选、商品、外汇				
private:
	std::vector<KContract>		m_vtForCur;			//外汇品种
	std::vector<KContract>		m_vtCom;			//商品品种
	std::vector<KContract>		m_vtFav;			//自选品种
	std::vector<KContract>		m_vtInternalCom;			//内盘品种
	std::vector<KContract>		m_vtExternalCom;			//外盘品种
	KContract					m_KContract;		//当前选中合约信息
	BOOL						m_bFirstInitData;	//是否初始化
	int							m_iHttpTimerID;		//定时器ID
};
