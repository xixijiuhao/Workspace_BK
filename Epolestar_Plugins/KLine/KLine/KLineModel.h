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
	RECT						m_rcCenter;					//画K线区域
	int							m_iCount;					//获取数据的总数量
	int							m_iStartIndex;				//起始点				
	int							m_iFocusIndex;				//选中点
	int							m_iEndIndex;				//结束点
	double						m_dUnit;					//坐标占用比
	SPriceType					m_dMaxPrice;				//显示的KLine最小值
	SPriceType					m_dMinPrice;				//显示KLine最大值
	std::vector<KLinePoint>		m_vtKLineDrawPoint;			//显示的位置信息
	std::vector<SHisQuoteData>	m_vtKLineDrawData;			//显示的数据信息
public:
	int						m_iTCTerm;						//当前选中TC周期(高频、当日、ST、MT)
	int						m_LastPricePoint;				//当前最新价坐标
	KLinePresenter*			m_clPreseter;
	KContract				m_KContract;
	SHisQuoteData			m_stTotalData[KLineMaxCount];	//获取MaxCount数据，用于指标数据计算；
	TCParamSpiData			m_httpData;                     //交易数据
	IntroductText			m_IntroductText;				//商品名称
	httpTools				m_httpTool;
	TCDataMap				m_TCDataMap;					//保存全部TC数据（Key = "Ticker|TC_Term"）
};


