#include "BaseInclude.h"
KLineModel::KLineModel()
{
	m_dUnit = 1;
	m_dMaxPrice = 0;
	m_dMinPrice = DBL_MAX;

	m_iEndIndex = 0;
	m_iStartIndex = 0;
	m_iFocusIndex = 0;

	m_bShowMainFrm = true;

	m_rcCenter = { 0 };
}

KLineModel::~KLineModel()
{
}

void KLineModel::SetPresenter(KLinePresenter* presenter)
{
	if (presenter) {
		m_clPreseter = presenter;
	}

	//注册一个回调函数，将KLineModel::OnHttpReceive函数 传给httpTool：：m_HttpReceive
	m_httpTool.RegistHttpRecv(std::bind(&KLineModel::OnHttpReceive, this, placeholders::_1, placeholders::_2));
	//注册一个回调函数，将KLineModel::OnHttpInitReceive函数 传给httpTool：：m_HttpInitReciver
	m_httpTool.RegistHttpInitRecv(std::bind(&KLineModel::OnHttpInitReceive, this, placeholders::_1));
	//新创建一个线程，调用 httpTools::Run() 从Http中取出数据，放入m_httpData，m_IntroductText
	m_httpTool.Start();
}
void KLineModel::SetKContract(KContract contract)
{
	memcpy(&m_KContract, &contract, sizeof(KContract));
	//这个是在干什么？
	SetEndIndex(0);
	InitDataFromTCMap();
	SSessionIdType sessionID = 0;
	if (g_pQuoteApi) {
		int iResult = g_pQuoteApi->SubHisQuote(
			contract.SubContractNo, 
			KLineUtil::GetKLineType(), 
			KLineUtil::GetKLineSlice(),
			KLineMaxCount, 
			sessionID, 
			m_clPreseter);

		if (iResult == 2) {
			UpdateTotalData();
		}
	}
}

void KLineModel::InitSubData(RECT center)
{
	m_rcCenter = center;
	m_rcCenter.right = center.right - KLineUtil::GetKLineTotalWidth() * 10;
	int Width = m_rcCenter.right - m_rcCenter.left;
	int klineCount = Width / KLineUtil::GetKLineTotalWidth();
	
	m_iEndIndex = 0;
	m_iStartIndex = m_iEndIndex + klineCount - 1;
	m_iFocusIndex = m_iEndIndex;

	//EndIndex > 获取总数量
	if (m_iStartIndex >= m_iCount) {
		m_iStartIndex = m_iCount - 1;
	}


	UpdateSubData();
	m_clPreseter->UpdateKLineView();
}
void KLineModel::UpdateSubData(BOOL updateView)
{
	int iHeight = m_rcCenter.bottom - m_rcCenter.top;
	if (!iHeight) { return;}
	m_vtKLineDrawData.clear();
	m_dMaxPrice = 0;
	m_dMinPrice = DBL_MAX;
	for (int index = m_iEndIndex; index <= m_iStartIndex; index++)
	{
		SHisQuoteData data = m_stTotalData[index];
		m_vtKLineDrawData.push_back(data);
		if (data.QHighPrice > m_dMaxPrice) {
			m_dMaxPrice = data.QHighPrice;
		}
		if (data.QLowPrice < m_dMinPrice) {
			m_dMinPrice = data.QLowPrice;
		}
	}
	UpdateSubDataPos();
}
void KLineModel::UpdateSubDataPos()
{
	m_vtKLineDrawPoint.clear();
	int iHeight = m_rcCenter.bottom - m_rcCenter.top;
	KLinePoint point = { 0 };
	SHisQuoteData data;
	int iSize = m_vtKLineDrawData.size();
	if (m_httpData.articleID && m_httpData.valid) {
		if (m_httpData.sup3 < m_dMinPrice) {
			m_dMinPrice = m_httpData.sup3;
		}
		if (m_httpData.res3 > m_dMaxPrice) {
			m_dMaxPrice = m_httpData.res3;
		}
	}
	m_dUnit = (m_dMaxPrice - m_dMinPrice) / (iHeight);
	for (int index = 0; index < iSize; index++)
	{
		data = m_vtKLineDrawData[index];
		point.HighY = m_rcCenter.top + iHeight - (data.QHighPrice - m_dMinPrice) / m_dUnit;
		point.LowY = m_rcCenter.top + iHeight - (data.QLowPrice - m_dMinPrice) / m_dUnit;
		point.OpenY = m_rcCenter.top + iHeight - (data.QOpeningPrice - m_dMinPrice) / m_dUnit;
		point.CloseY = m_rcCenter.top + iHeight - (data.QLastPrice - m_dMinPrice) / m_dUnit;
		point.x = (iSize - index - 1) * KLineUtil::GetKLineTotalWidth() + m_rcCenter.left;
		m_vtKLineDrawPoint.push_back(point);
	}
	//保存最新价坐标
	data = m_stTotalData[0];
	m_LastPricePoint = m_rcCenter.top + iHeight - (data.QLastPrice - m_dMinPrice) / m_dUnit;
}
void KLineModel::Slide(int changePix)
{

}
void KLineModel::SetEndIndex(int endIndex)
{
	int LineCount = (m_rcCenter.right - m_rcCenter.left) / (KLineUtil::GetKLineTotalWidth());
	if (endIndex < 0 ) {
		m_iEndIndex = 0;
		m_iStartIndex = m_iEndIndex + LineCount;
		if (m_iStartIndex >= m_iCount) {
			m_iStartIndex = m_iCount - 1;
		}
	}
	else if (endIndex >= m_iCount) {
		m_iStartIndex = m_iCount - 1;
		m_iEndIndex = m_iStartIndex - LineCount;
		if (m_iEndIndex < 0) {
			m_iEndIndex = 0;
		}
	}
	else {
		m_iEndIndex = endIndex;
		m_iStartIndex = m_iEndIndex + LineCount;
		if (m_iStartIndex >= m_iCount) {
			m_iStartIndex = m_iCount - 1;
			m_iEndIndex = m_iStartIndex - LineCount;
			if (m_iEndIndex < 0) {
				m_iEndIndex = 0;
			}
		}
	}
}
int KLineModel::GetYPosByPrice(SPriceType price)
{
	int pos = m_rcCenter.top + (m_rcCenter.bottom - m_rcCenter.top) - (price - m_dMinPrice) / m_dUnit;
	return pos;
}
SPriceType KLineModel::GetPriceByYPos(int Ypos)
{
	SPriceType price = m_dUnit * (m_rcCenter.top + (m_rcCenter.bottom - m_rcCenter.top) - Ypos) + m_dMinPrice;
	return price;
}
int KLineModel::GetXIndex(int x)
{
	int index = (x - KLineView_LeftWidth) / KLineUtil::GetKLineTotalWidth();
	if (index < m_vtKLineDrawPoint.size()) {
		return (m_vtKLineDrawPoint.size() - 1 - index);
	}
	return m_vtKLineDrawPoint.size() - 1;
}
int KLineModel::GetDateTimeIndex(SDateTimeType dateTime)
{
	for (int index = 0 ; index < m_vtKLineDrawData.size(); index++)
	{
		if (m_vtKLineDrawData[index].DateTimeStamp >= dateTime && index + 1 < m_vtKLineDrawData.size()  && m_vtKLineDrawData[index + 1].DateTimeStamp < dateTime) {
			return index;
		}
	}
	return -1;
}
void KLineModel::OnHisQuote(const SContract* contract, const SKLineTypeType klinetype)
{
	switch (klinetype)
	{
	case S_KLINE_TICK:
		UpdateTotalData();
		break;
	case S_KLINE_MINUTE:
		UpdateTotalData();
		break;
	case S_KLINE_DAY:
		UpdateTotalData();
		break;
	default:
		break;
	}
}
void KLineModel::OnHttpReceive(TCParamSpiData data, IntroductText text)
{
	if (strcmp(m_KContract.TCTicker, data.ticker) == 0) {
		memcpy(&m_httpData, &data, sizeof(TCParamSpiData));
	}
	m_IntroductText = text;
	PostMessage(m_clPreseter->GetHwnd(), SSWM_HTTP_SPI, 0, 0);
}
void KLineModel::InitDataFromTCMap()
{
	//更新目前显示的品种TC数据
	char key[256] = { 0 };
	TCTerm term = { 0 };
	KLineUtil::GetTCTerm(m_iTCTerm, term, sizeof(TCTerm));
	sprintf_s(key, "%s|%s", m_KContract.TCTicker, term);
	TCDataMap::iterator iter = m_TCDataMap.find(key);
	if (iter == m_TCDataMap.end()) {
		m_httpData.valid = false;
		m_IntroductText.clear();
		char tips[256] = { 0 };
		
		if (m_TCDataMap.size() == 0) {
			WCharToMByte(g_pLanguageApi->LangText(GettingDataStrID), tips, sizeof(tips));
			m_IntroductText.push_back(tips);
		}
		else {
			WCharToMByte(g_pLanguageApi->LangText(NoDataTipStrID), tips, sizeof(tips));
			m_IntroductText.push_back(tips);
		}
		
	}
	else {
		memcpy(&m_httpData, &iter->second.data, sizeof(TCParamSpiData));
		m_IntroductText = iter->second.text;
	}
}
void KLineModel::OnHttpInitReceive(TCDataMap& DataMap)
{
	m_TCDataMap = DataMap;
	InitDataFromTCMap();
	PostMessage(m_clPreseter->GetHwnd(), SSWM_HTTP_SPI, 0, 0);
}
void KLineModel::UpdateTotalData()
{
	if (g_pQuoteApi) {
		memset(&m_stTotalData, 0, sizeof(m_stTotalData));
		m_iCount = g_pQuoteApi->GetHisQuote(m_KContract.SubContractNo, KLineUtil::GetKLineType(), KLineUtil::GetKLineSlice(), 1, 0, m_stTotalData, KLineMaxCount);
		if (m_iCount) {
			UpdateSubData();
			if (m_bShowMainFrm) {
				m_clPreseter->getMainFrm()->CreateView();
				m_bShowMainFrm = false;
			}
			if (m_rcCenter.bottom) {
				m_clPreseter->UpdateKLineView();
			}
		}
	}
}
void KLineModel::GetHttpData(TCProduct product, TCTerm term, GetHttpType type)
{
	//获取http数据(可变参数 1.product, 2.term)
	TCParams params;
	memset(&params, 0, sizeof(params));
	strcpy_s(params.product_type, sizeof(params.product_type), TC_typenull);
	strcpy_s(params.product, sizeof(params.product), product);
	strcpy_s(params.term, sizeof(params.term), term);
	strcpy_s(params.day_slice, sizeof(params.day_slice), "1");
	strcpy_s(params.last, sizeof(params.last), "true");
	strcpy_s(params.partnerNo, sizeof(params.partnerNo), TC_esunnyNo);
	strcpy_s(params.token, sizeof(params.token), TC_esunnyToken);

	//多语言
	if (g_language == CHS){
		strcpy_s(params.culture, sizeof(params.culture), TC_zh_CN);
	}
	else if (g_language == CHT) {
		strcpy_s(params.culture, sizeof(params.culture), TC_zh_TW);
	}else if (g_language == ENU)
	{
		strcpy_s(params.culture, sizeof(params.culture), TC_en_US);
	}
	
	if (type == AsynGetAll) 
	{
		//异步请求全部数据
		m_httpTool.AsynGetAllInterNetURLText();
	}
	else if (type == SynGet) 
	{
		//同步请求数据,请求之前先将valid置为false
		m_httpData.valid = false;
		m_httpTool.GetInterNetURLText(params, m_httpData, m_IntroductText);
		UpdateSubData(true);
		m_clPreseter->UpdateKLineView();
	}
	else if (type == AsynGet)
	{
		//异步请求单组数据
		m_httpData.valid = false;
		m_IntroductText.clear();
		m_httpTool.AsynGetInterNetURLText(params);
	}
}