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
	//��ʷ������֪ͨ ��֪ͨ���ݽ����仯��ʹ��get������ȡ����
	void __stdcall OnHisVolatility(const SContractNoType contractno, const SKLineTypeType klinetype, const SSessionIdType sessionid) {};
	//��ʷ����֪ͨ ��֪ͨ���ݽ����仯��ʹ��get������ȡ����
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

	BOOL						m_bChangeExchangeData;	//�Ƿ��ʼ��
	std::string					m_sCurExchangeNo;		//��ǰѡ�еĽ�����

private:
	KLineModel*			m_KLineModel;				//Model������	
	TMainFrm*			m_MainFrm;					//��������
	KLineView*			m_KLineView;				//KLine��
	KLineSelectTab*		m_KLineSelectTab;			//ѡTCTerm��
	KLineIntroductView*	m_KLineIntroduct;			//����˵����
	KListView*			m_KListView;				//�б�ѡ����
	CContractInfoListView* m_ContractInfoListView;				//��Լѡ����
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
	std::vector<ACContract>		m_vtForCur;			//���Ʒ��
	std::vector<ACContract>		m_vtCom;			//��ƷƷ��
	std::vector<ACContract>		m_vtFav;			//��ѡƷ��
	std::vector<ACContract>		m_vtInternalCom;			//����Ʒ��
	std::vector<ACContract>		m_vtExternalCom;			//����Ʒ��

	std::vector<string>		m_vtInternalEx;			//���̽�����
	std::vector<string>		m_vtExternalEx;			//���̽�����

	ACContract					m_KContract;		//��ǰѡ�к�Լ��Ϣ
	
	int							m_iHttpTimerID;		//��ʱ��ID

};
