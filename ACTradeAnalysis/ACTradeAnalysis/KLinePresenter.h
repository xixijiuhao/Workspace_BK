#pragma once

class ACAnalysisPresenter : public ISHisQuoteSpi, public TIxFrm
{
public:
	ACAnalysisPresenter();
	~ACAnalysisPresenter();
	static void OnMenuClick(const unsigned int MenuIndex, const HWND PastLife);

protected:
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	void __stdcall OnHisQuote(const SContractNoType contractno, const SKLineTypeType klinetype, const SSessionIdType sessionid);
	//历史波动率通知 仅通知数据结束变化，使用get函数获取数据
	void __stdcall OnHisVolatility(const SContractNoType contractno, const SKLineTypeType klinetype, const SSessionIdType sessionid) {};
	//历史行情通知 仅通知数据结束变化，使用get函数获取数据
	void __stdcall OnHisQuoteTimeLine(const SContractNoType contractno, const SDateType MTradeDate, const SSessionIdType sessionid) {};

public:
	void UpdateKLineView(ACContract &contract);
	void ShowMainFrm() { m_MainFrm->CreateView(); };
	void ShowSubView(MainFrmRect &rect);
	void ChangeSubViewFrame(MainFrmRect &rect);
	void InitSubViewData(RECT CenterKLineRect) {};
	const ACContract GetContract();
	ACContract& GetKContract() { return m_KContract; }

public:
	TMainFrm* getMainFrm();
	KLineModel* getKLineModel();
	CContractInfoListView* getCContractInfoListView();

public:
	std::map<string, string>    m_mapContractSymbol;
	std::map<string, string>    m_mapSymbolContract;
	std::map<string, ContractTransInfo>    m_mapSymbolNameContractInfo;

	BOOL						m_bChangeExchangeData;	//是否初始化
	std::string					m_sCurExchangeNo;		//当前选中的交易所

private:
	KLineModel*			m_KLineModel;				//Model数据类	
	TMainFrm*			m_MainFrm;					//主界面类
	KLineView*			m_KLineView;				//KLine类
	KLineSelectTab*		m_KLineSelectTab;			//选TCTerm类
	KLineIntroductView*	m_KLineIntroduct;			//介绍说明类
	KListView*			m_KListView;				//列表选择类
	CContractInfoListView* m_ContractInfoListView;				//合约选择类
private:
	void OnTabChange(int index);
	void OnClickListItem(string exchangeNo);
	void OnClickContractListItem(ContractInfoViewItemData contractItemData);
	void GetTCData();
	void GetTCDataFromMap();
	void GetTCDataSpi();
	void UpdateView();
	void LoadFileData();
	void LoadFileDataNew();

private:
	std::vector<ACContract>		m_vtForCur;			//外汇品种
	std::vector<ACContract>		m_vtCom;			//商品品种
	std::vector<ACContract>		m_vtFav;			//自选品种
	std::vector<ACContract>		m_vtInternalCom;			//内盘品种
	std::vector<ACContract>		m_vtExternalCom;			//外盘品种

	std::vector<string>		m_vtInternalEx;			//内盘交易所
	std::vector<string>		m_vtExternalEx;			//外盘交易所

	ACContract					m_KContract;		//当前选中合约信息
	
	int							m_iHttpTimerID;		//定时器ID

};
