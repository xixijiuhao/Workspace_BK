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
	//��ʷ������֪ͨ ��֪ͨ���ݽ����仯��ʹ��get������ȡ����
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
	KLineModel*			m_KLineModel;				//Model������	
	TMainFrm*			m_MainFrm;					//��������
	KLineView*			m_KLineView;				//KLine��
	KLineSelectTab*		m_KLineSelectTab;			//ѡTCTerm��
	KLineIntroductView*	m_KLineIntroduct;			//����˵����
	KListView*			m_KListView;				//�б�ѡ����
private:
	void OnTabChange(int index);
	void OnClickListItem(KContract p);
	void GetTCData();
	void GetTCDataFromMap();
	void GetTCDataSpi();
	void UpdateView();
	void LoadFileData();							//���μ�����ѡ����Ʒ�����				
private:
	std::vector<KContract>		m_vtForCur;			//���Ʒ��
	std::vector<KContract>		m_vtCom;			//��ƷƷ��
	std::vector<KContract>		m_vtFav;			//��ѡƷ��
	KContract					m_KContract;		//��ǰѡ�к�Լ��Ϣ
	BOOL						m_bFirstInitData;	//�Ƿ��ʼ��
	int							m_iHttpTimerID;		//��ʱ��ID
};
