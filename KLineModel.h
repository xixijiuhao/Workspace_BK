#pragma once

enum GetHttpType
{
	SynGet = 0,
	AsynGet ,
	AsynGetAll
};
class KLinePresenter;
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
	void SetPresenter(KLinePresenter* presenter);
	void SetKContract(KContract contract);
	void GetHttpData(TCProduct product, TCTerm term, GetHttpType type  = AsynGetAll);
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
	void OnHttpReceive(TCParamSpiData data, IntroductText text);
	void OnHttpInitReceive(TCDataMap& DataMap);
	void InitDataFromTCMap();
private:
	void UpdateTotalData();
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
	KLinePresenter*			m_clPreseter;
	KContract				m_KContract;
	SHisQuoteData			m_stTotalData[KLineMaxCount];	//��ȡMaxCount���ݣ�����ָ�����ݼ��㣻
	TCParamSpiData			m_httpData;                     //��������
	IntroductText			m_IntroductText;				//��Ʒ����
	httpTools				m_httpTool;
	TCDataMap				m_TCDataMap;					//����ȫ��TC���ݣ�Key = "Ticker|TC_Term"��
};


