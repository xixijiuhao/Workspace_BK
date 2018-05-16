#include "BaseInclude.h"
vector<KLinePresenter*> g_pOrderVec;
typedef wchar_t path[101];
path FILEPATH[3] = {
//L"\\config\\TC\\TC_Commodity.pri",
L"\\config\\TC\\TC_Internal.pri",
L"\\config\\TC\\TC_External.pri",
L"\\config\\TC\\TC_Foreign_Currency.pri" };

//修改读取文件、初始化ShowsubView初始化ListViewMap
KLinePresenter::KLinePresenter():m_bFirstInitData(false), m_iHttpTimerID(0)
{
	//LOG怎么打？
	//FindWindow TMainFrame 是什么类？
	//在这里完成了KLineView的创建，此时已经将K线图画出来了
	CreateFrm(L"KLinePresenter", FindWindow(EMainFrmName, NULL), NULL);
	m_MainFrm = new TMainFrm();
	//new 一个 Model 未做特殊处理
	m_KLineModel = new KLineModel();
	//new 一个 Model 未做特殊处理
	m_KLineView = new KLineView();
	//new 一个 SelectTab 构造函数会新建tab标签页 其他未做特殊处理
	m_KLineSelectTab = new KLineSelectTab();
	//new 一个 Introduct 未做特殊处理
	m_KLineIntroduct = new KLineIntroductView();
	//new 一个 Introduct 未做特殊处理
	m_KListView = new KListView();

	//Set TMainFrm::m_Presenter 其他未做特殊处理
	m_MainFrm->setPresenter(this);
	//创建线程获取 Data
	m_KLineModel->SetPresenter(this);
	//Set KLineView::m_clPresenter 其他未做特殊处理
	m_KLineView->SetPresenter(this);
	//Set KLineView::m_clModel 其他未做特殊处理
	m_KLineView->SetKLineModel(m_KLineModel);

	//注册函数指针
	m_KLineSelectTab->RegistFunc(bind(&KLinePresenter::OnTabChange, this, placeholders::_1));
	m_KListView->RegistFunc(bind(&KLinePresenter::OnClickListItem, this, placeholders::_1));

	//从配置文件中读取配置信息，将信息存入m_vtCom
	//m_vtFav是什么结构？为什么没有存？
	LoadFileData();
	if (m_vtFav.size() > 0) {
		m_KContract = m_vtFav[0];
	}
	else if (m_vtInternalCom.size() > 0) {
		m_KContract = m_vtInternalCom[0];
	}
}

KLinePresenter::~KLinePresenter()
{
	delete(m_KLineModel);
	delete(m_MainFrm);
	delete(m_KLineView);
	delete(m_KLineSelectTab);
}
TMainFrm* KLinePresenter::getMainFrm()
{
	if (m_MainFrm) {
		return m_MainFrm;
	}
	else {
		m_MainFrm = new TMainFrm;
		return m_MainFrm;
	}
}
KLineModel* KLinePresenter::getKLineModel()
{
	if (m_KLineModel) {
		return m_KLineModel;
	}
	else {
		m_KLineModel = new KLineModel;
		return m_KLineModel;
	}
}
void KLinePresenter::OnMenuClick(const unsigned int MenuIndex, const HWND PastLife)
{
	//new一个Presenter对象，在构造函数里会new Model类 和 View类 并新建线程从http中获取数据
	KLinePresenter* present = new KLinePresenter;

	present->getKLineModel()->SetKContract(present->GetKContract());
	present->m_iHttpTimerID = 1;
	SetTimer(present->GetHwnd(), present->m_iHttpTimerID, 600000, NULL);
	PostMessage(present->GetHwnd(), WM_TIMER, 0, 0);
	g_pOrderVec.push_back(present);
}
void KLinePresenter::ShowSubView(MainFrmRect &rect)
{
	//KLine 开始画图
	m_KLineView->CreateView(m_MainFrm->GetHwnd(), rect.KLineChartRect);
	m_KLineModel->InitSubData(m_KLineView->GetKLineChartRect().CenterKLineRect);
	//termTab
	m_KLineSelectTab->CreateView(m_MainFrm->GetHwnd(), KLineUtil::CRectToRect(rect.TopRect));
	InvalidateRect(m_KLineSelectTab->GetHwnd(), 0, true);
	UpdateWindow(m_KLineSelectTab->GetHwnd());
	InvalidateRect(m_KLineSelectTab->GetHwnd(), 0, true);
	UpdateWindow(m_KLineSelectTab->GetHwnd());
	//Introduct;
	m_KLineIntroduct->SetText(m_KLineModel->m_IntroductText);
	m_KLineIntroduct->CreateView(m_MainFrm->GetHwnd(), KLineUtil::CRectToRect(rect.IntroductRect));
	//RightList
	std::map<int, std::vector<KContract>> list;
	list.insert(std::make_pair(0, m_vtInternalCom));
	list.insert(std::make_pair(1, m_vtExternalCom));
	list.insert(std::make_pair(2, m_vtForCur));
	m_KListView->SetTotalData(list);
	m_KListView->CreateView(m_MainFrm->GetHwnd(), KLineUtil::CRectToRect(rect.CommodityListRect));
}
void KLinePresenter::ChangeSubViewFrame(MainFrmRect &rect)
{
	//图表
	m_KLineView->MoveWindow(rect.KLineChartRect);
	m_KLineModel->InitSubData(m_KLineView->GetKLineChartRect().CenterKLineRect);
	//termTab
	m_KLineSelectTab->MoveWindow(KLineUtil::CRectToRect(rect.TopRect));
	//Introduct
	m_KLineIntroduct->MoveWindow(KLineUtil::CRectToRect(rect.IntroductRect));
	//RightList
	m_KListView->MoveWindow(KLineUtil::CRectToRect(rect.CommodityListRect));
}
void KLinePresenter::LoadFileData()
{
	TFile file;
	string buf = "";
	string Qcom = "";
	string TCcom = "";
	string::size_type pos;
	TCHAR filePath[250] = { 0 };
	SContract* contract[1] = { 0 };
	SContract* underLay = NULL;
	KContract KCon;
	memset(&KCon, 0, sizeof(KContract));
	//三个文件目录 （自选、商品、外汇）
	for (int index = 0; index < sizeof(FILEPATH)/sizeof(path); index++)
	{
		file.GetFilePath(filePath, ARRAYSIZE(filePath), FILEPATH[index]);
		file.Open(LoadRC::unicode_to_ansi(filePath).c_str(), "r");
		while (true) {
			if (!file.ReadOneLine(buf))
				break;
			buf.erase(std::remove(buf.begin(), buf.end(), '\n'), buf.end());
			pos = buf.find(",");
			//endPos = buf.find("\n");
			if (pos == string::npos)
				continue;
			Qcom = buf.substr(0, pos);
			TCcom = buf.substr(pos + 1, string::npos);
			
			int icount = g_pQuoteApi->GetContractData("", Qcom.c_str(), contract, 1, false);
			//区分主力合约和非主力合约
			if (icount) 
			{
				strcpy_s(KCon.SubContractNo, Qcom.c_str());
				underLay = g_pQuoteApi->GetContractUnderlay(contract[0]->ContractNo);
				if (underLay) 
				{
					KCon.pContract = underLay;
				}
				else 
				{
					KCon.pContract = contract[0];
				}
				strcpy_s(KCon.TCTicker, sizeof(KCon.TCTicker), TCcom.c_str());
				if (index == 0) 
				{
					wcsncpy_s(KCon.plate, g_pLanguageApi->LangText(InternalID), sizeof(KCon.plate));
				}
				else if (index == 1) 
				{
					wcsncpy_s(KCon.plate, g_pLanguageApi->LangText(ExternalID), sizeof(KCon.plate));
				}
				else if (index == 2) 
				{
					wcsncpy_s(KCon.plate, g_pLanguageApi->LangText(ForeignExchangeStrID), sizeof(KCon.plate));
				}
				else 
				{
					wcsncpy_s(KCon.plate, L"", sizeof(KCon.plate));
				}
				char temp[101] = { 0 };
				g_pQuoteApi->GetContractCode(KCon.pContract->ContractNo, temp);
				wstring code =  CConvert::UTF8ToUnicode(temp);
				swprintf_s(KCon.contractCode, L"%s", code.c_str());
				SRetType ires =  g_pQuoteApi->GetContractName(KCon.pContract->ContractNo, temp);
				code = CConvert::UTF8ToUnicode(temp);
				swprintf_s(KCon.contractWName, L"%s", code.c_str());
				swprintf_s(KCon.contractShowName, L"%s / %s", KCon.contractWName, KCon.contractCode);
				if (index == 0)
				{
					m_vtInternalCom.push_back(KCon);
				}
				else if (index == 1)
				{
					m_vtExternalCom.push_back(KCon);
				}
				else if (index == 2) 
				{
					m_vtForCur.push_back(KCon);
				}
				else if (index == 3)
				{
					m_vtForCur.push_back(KCon);
				}
			}
		}
	}
}
void KLinePresenter::UpdateKLineView()
{
	m_KLineView->UpdateKLineView();
	m_KLineIntroduct->SetText(m_KLineModel->m_IntroductText);
	InvalidateRect(m_KLineIntroduct->GetHwnd(), nullptr, false);
}
const SContract* KLinePresenter::GetContract(){
	return m_KContract.pContract;
}
LRESULT KLinePresenter::WndProc(UINT message, WPARAM wParam, LPARAM lParam){
	switch (message)
	{
	case WM_ONQUOTE:
		m_KLineModel->OnHisQuote(m_KContract.pContract, wParam);
		break;
	case WM_TIMER:
		GetTCData();
		break;
	case SSWM_HTTP_SPI:
		GetTCDataSpi();
		break;
	case WM_NCDESTROY:
	{
		return PROCESSED;
	}
		break;
	default:
		break;
	}
	return NOT_PROCESSED;
}
void KLinePresenter::OnHisQuote(const SContractNoType contractno, const SKLineTypeType klinetype, const SSessionIdType sessionid) {
	if (strcmp(m_KContract.SubContractNo, contractno) == 0) {
		PostMessage(m_Hwnd, WM_ONQUOTE, (WPARAM)klinetype, (LPARAM)sessionid);	
	}	
	if (klinetype == S_KLINE_MINUTE) {
		OutputDebugStringA(contractno);
	}
};
void KLinePresenter::OnClickListItem(KContract p)
{
	m_KContract = p;
	m_KLineModel->SetKContract(p);
}
void KLinePresenter::OnTabChange(int index)
{
	if (m_KLineModel->m_iTCTerm != index)
	{
		m_KLineModel->m_iTCTerm = index;
		if (m_KLineModel->m_iTCTerm == 0) {
			KLineUtil::SetKLineType(S_KLINE_MINUTE);
			KLineUtil::SetKLineSlice(KLine_Min15);
		}
		else if (m_KLineModel->m_iTCTerm == 1) {
			KLineUtil::SetKLineType(S_KLINE_MINUTE);
			KLineUtil::SetKLineSlice(KLine_Min30);
		}
		else if (m_KLineModel->m_iTCTerm == 2) {
			KLineUtil::SetKLineType(S_KLINE_DAY);
			KLineUtil::SetKLineSlice(KLine_Day);
		}
		else if (m_KLineModel->m_iTCTerm == 3) {
			KLineUtil::SetKLineType(S_KLINE_DAY);
			KLineUtil::SetKLineSlice(KLine_Day7);
		}
		m_KLineModel->SetKContract(m_KContract);
	}
}
//主要三个地方请求数据：
//1.初始化；
//2.十分钟请求一次Timer；
//3.点击切换TC周期；
//4.点击列表切换品种
void KLinePresenter::GetTCDataFromMap()
{
	m_KLineModel->InitDataFromTCMap();
	m_KLineModel->UpdateSubData(true);
	UpdateKLineView();
}
void KLinePresenter::GetTCData()
{
	TCProduct product;
	TCTerm term;
	m_KLineModel->m_httpData.valid = false;
	strcpy_s(product, sizeof(product), m_KContract.TCTicker);
	KLineUtil::GetTCTerm(m_KLineModel->m_iTCTerm, term, sizeof(TCTerm));
	m_KLineModel->GetHttpData(product, term, AsynGetAll);
}
void KLinePresenter::GetTCDataSpi()
{
	UpdateView();
}
void KLinePresenter::UpdateView()
{
	m_KLineModel->UpdateSubData(true);
	UpdateKLineView();
}