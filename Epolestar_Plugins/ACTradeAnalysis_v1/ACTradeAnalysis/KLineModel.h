#pragma once

enum GetHttpType
{
	SynGet = 0,
	AsynGet ,
	AsynGetAll
};
class ACAnalysisPresenter;
class KLineModel
{
public:
	KLineModel();
	~KLineModel();
public:
	void InitSubData(RECT centerRect);
	void UpdateSubData(BOOL updateView = true);
	void UpdateSubDataPos();
	void Slide(int changePix);
	void SetPresenter(ACAnalysisPresenter* presenter);
	void SetKContract(ACContract contract);
	void SetContractInfoItem(ContractInfoViewItemData contractItemData);
	void GetHttpData(AC_Service_Type product, GetHttpType type  = AsynGetAll);
	inline int GetSubSize() { return (m_iStartIndex - m_iEndIndex + 1); }
	inline SHisQuoteData& StartData() { return m_stTotalData[m_iStartIndex]; }
	inline SHisQuoteData& EndData() { return m_stTotalData[m_iEndIndex]; }
	inline SHisQuoteData& FocusData() { return m_stTotalData[m_iFocusIndex]; }

public:
	void SetEndIndex(int endIndex);
	int GetYPosByPrice(SPriceType price);
	SPriceType GetPriceByYPos(int Ypos);
	int GetXIndex(int x);
	int GetDateTimeIndex(SDateTimeType dateTime);
public:
	void OnHisQuote(const SContract* contract, const SKLineTypeType klinetype);
	void OnHttpReceive(ACSpiData data);
	void OnHttpInitReceive(ACDataMap& DataMap);
	void InitDataFromTCMap();
	void InitContractFromTCMap();
	void UpdateTotalData();
	std::string GetSymbolName(std::string symbol);
public:
	BOOL						m_bShowMainFrm;
	RECT						m_rcCenter;					//��K������
	int							m_iCount;					//��ȡ���ݵ�������
	int							m_iStartIndex;				//��ʼ��				
	int							m_iFocusIndex;				//ѡ�е�
	int							m_iEndIndex;				//������
	double						m_dUnit;					//����ռ�ñ�
	SPriceType					m_dMaxPrice;				//��ʾ��KLine��Сֵ
	SPriceType					m_dMinPrice;				//��ʾKLine���ֵ
	std::vector<KLinePoint>		m_vtKLineDrawPoint;			//��ʾ��λ����Ϣ
	std::vector<SHisQuoteData>	m_vtKLineDrawData;			//��ʾ��������Ϣ
public:
	int						m_iTCTerm;						//��ǰѡ��TC����(��Ƶ�����ա�ST��MT)
	int						m_LastPricePoint;				//��ǰ���¼�����
	ACAnalysisPresenter*	m_clPreseter;
	ACContract				m_KContract;
	SHisQuoteData			m_stTotalData[KLineMaxCount];	//��ȡMaxCount���ݣ�����ָ�����ݼ��㣻
	ACSpiData				m_httpData;                     //��������
	vector<string>			m_IntroductText;				//��Ʒ����
	httpTools				m_httpTool;
	ACDataMap				m_TCDataMap;					//����ȫ��AC���ݣ�Key = ("%s|%s", Symbol, pattern)��
	ExchangeNoACDataMap     m_ExchangeNoACDataMap;			//�������е��Ѿ��������AC����
};


