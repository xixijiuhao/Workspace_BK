#include "BaseInclude.h"
typedef wchar_t path[101];
path FILEPATH[3] = {
L"\\config\\AC\\AC_Internal.pri",
L"\\config\\AC\\AC_External.pri" };  //L"\\config\\TC\\TC_Commodity.pri",   //,L"\\config\\AC\\TC_Foreign_Currency.pri"

path CONTRACTTANSPATH = L"\\config\\AC\\ContractTrans.pri"; 
path CONTRACTTANSPATH_NEW = L"\\config\\AC\\ContractTrans_new.pri";

std::string MONTHSTR[] =
{
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
};

//�޸Ķ�ȡ�ļ�����ʼ��ShowsubView��ʼ��ListViewMap
ACAnalysisPresenter::ACAnalysisPresenter():m_bChangeExchangeData(true), m_iHttpTimerID(0)
{
	CreateFrm(L"ACAnalysisPresenter", FindWindow(EMainFrmName, NULL), NULL);
	m_MainFrm = new TMainFrm();
	m_KLineModel = new KLineModel();
	m_KLineView = new KLineView();
	m_KLineSelectTab = new KLineSelectTab();
	m_ContractInfoListView = new CContractInfoListView();
	m_KLineIntroduct = new KLineIntroductView();
	m_KListView = new KListView();

	m_MainFrm->setPresenter(this);
	//�����̻߳�ȡ Data
	m_KLineModel->SetPresenter(this);
	m_KLineView->SetPresenter(this);
	m_KLineView->SetKLineModel(m_KLineModel);
	m_ContractInfoListView->SetPresenter(this);

	//ע�ắ��ָ��
	m_KLineSelectTab->RegistFunc(bind(&ACAnalysisPresenter::OnTabChange, this, placeholders::_1));
	m_KListView->RegistFunc(bind(&ACAnalysisPresenter::OnClickListItem, this, placeholders::_1));
	m_ContractInfoListView->RegistFunc(bind(&ACAnalysisPresenter::OnClickContractListItem, this, placeholders::_1));

	//��ȡ�ļ�����ContractNo��symbol�ֶεĶ�Ӧ��ϵ
	LoadFileDataNew();

	//�洢������List
	m_vtInternalEx.push_back(Ex_CZCE);
	m_vtInternalEx.push_back(Ex_SHFE);
	m_vtInternalEx.push_back(Ex_DCE);
	m_vtInternalEx.push_back(Ex_CFFEX);

	m_vtExternalEx.push_back(Ex_CFFEX);

	m_sCurExchangeNo = Ex_CZCE;
	m_iHttpTimerID = 1;
	SetTimer(m_Hwnd, m_iHttpTimerID, 600000, NULL);
	PostMessage(m_Hwnd, WM_TIMER, 0, 0);
}

ACAnalysisPresenter::~ACAnalysisPresenter()
{
	delete(m_KLineModel);
	delete(m_MainFrm);
	delete(m_KLineView);
	delete(m_KLineSelectTab);
}
TMainFrm* ACAnalysisPresenter::getMainFrm()
{
	if (m_MainFrm) 
	{
		return m_MainFrm;
	}
	else 
	{
		m_MainFrm = new TMainFrm;
		return m_MainFrm;
	}
}
KLineModel* ACAnalysisPresenter::getKLineModel()
{
	if (m_KLineModel) 
	{
		return m_KLineModel;
	}
	else 
	{
		m_KLineModel = new KLineModel;
		return m_KLineModel;
	}
}

CContractInfoListView* ACAnalysisPresenter::getCContractInfoListView()
{
	if (m_ContractInfoListView)
	{
		return m_ContractInfoListView;
	}
	else
	{
		m_ContractInfoListView = new CContractInfoListView;
		return m_ContractInfoListView;
	}
}

void ACAnalysisPresenter::OnMenuClick(const unsigned int MenuIndex, const HWND PastLife)
{
	//����˵�ʱ��newһ��Presenter�����ڹ��캯�����new Model�� �� View�� ���½��̴߳�http�л�ȡ����
	ACAnalysisPresenter* present = new ACAnalysisPresenter;
	present->getMainFrm()->CreateView();
	ShowWindow(present->getMainFrm()->GetHwnd(),SW_SHOW);
}

void ACAnalysisPresenter::ShowSubView(MainFrmRect &rect)
{
	//KLine ��ʼ��ͼ
	m_KLineView->CreateView(m_MainFrm->GetHwnd(), rect.KLineChartRect);
	m_KLineModel->InitSubData(m_KLineView->GetKLineChartRect().CenterKLineRect);

	//ģʽѡ��
	m_KLineSelectTab->CreateView(m_MainFrm->GetHwnd(), KLineUtil::CRectToRect(rect.TopRect));
	InvalidateRect(m_KLineSelectTab->GetHwnd(), 0, true);
	UpdateWindow(m_KLineSelectTab->GetHwnd());

	//����ָ��չʾ;
	char key[256] = { 0 };
	sprintf_s(key, "%s|%s", m_KContract.commoditySymbol, m_KContract.pattern);
	auto spiDataIter = m_KLineModel->m_TCDataMap.find(key);
	if (spiDataIter != m_KLineModel->m_TCDataMap.end())
	{
		m_KLineIntroduct->SetText(spiDataIter->second);
	}
	m_KLineIntroduct->CreateView(m_MainFrm->GetHwnd(), KLineUtil::CRectToRect(rect.IntroductRect));

	//ContractListView
	m_ContractInfoListView->SetTotalData(m_KLineModel->m_TCDataMap);
	m_ContractInfoListView->CreateView(m_MainFrm->GetHwnd(), KLineUtil::CRectToRect(rect.ContractInfoRect));

	//������List
	std::map<int, std::vector<string>> list;
	list.insert(std::make_pair(0, m_vtInternalEx));
	list.insert(std::make_pair(1, m_vtExternalEx));
	m_KListView->SetTotalData(list);
	m_KListView->CreateView(m_MainFrm->GetHwnd(), KLineUtil::CRectToRect(rect.CommodityListRect));
}

void ACAnalysisPresenter::ChangeSubViewFrame(MainFrmRect &rect)
{
	//ͼ��
	m_KLineView->MoveWindow(rect.KLineChartRect);
	m_KLineModel->InitSubData(m_KLineView->GetKLineChartRect().CenterKLineRect);
	m_KLineSelectTab->MoveWindow(KLineUtil::CRectToRect(rect.TopRect));
	m_KLineIntroduct->MoveWindow(KLineUtil::CRectToRect(rect.IntroductRect));
	m_ContractInfoListView->MoveWindow(KLineUtil::CRectToRect(rect.ContractInfoRect));
	m_KListView->MoveWindow(KLineUtil::CRectToRect(rect.CommodityListRect));
}

void ACAnalysisPresenter::LoadFileData()
{
	TFile file;
	string buf = "";
	string sContractNo = "";
	string sACSymbol = "";
	string::size_type pos;
	TCHAR filePath[250] = { 0 };

	file.GetFilePath(filePath, ARRAYSIZE(filePath), CONTRACTTANSPATH);
	file.Open(LoadRC::unicode_to_ansi(filePath).c_str(), "r");
	while (true)
	{
		if (!file.ReadOneLine(buf))
		{
			break;
		}

		buf.erase(std::remove(buf.begin(), buf.end(), '\n'), buf.end());
		pos = buf.find(",");
		if (pos == string::npos)
			continue;
		sContractNo = buf.substr(0, pos);
		sACSymbol = buf.substr(pos + 1, string::npos);

		if (!sContractNo.empty() && !sACSymbol.empty())
		{
			m_mapContractSymbol[sContractNo] = sACSymbol;
			m_mapSymbolContract[sACSymbol] = sContractNo;
		}
	}
}

void ACAnalysisPresenter::LoadFileDataNew()
{
	TFile file;
	string buf = "";
	string sContractNo = "";
	string sACSymbol = "";
	string::size_type pos;
	TCHAR filePath[250] = { 0 };

	SContract* contract[1] = { 0 };
	SContract* underLay = NULL;

	file.GetFilePath(filePath, ARRAYSIZE(filePath), CONTRACTTANSPATH_NEW);
	file.Open(LoadRC::unicode_to_ansi(filePath).c_str(), "r");
	while (true) 
	{
		if (!file.ReadOneLine(buf))
		{
			break;
		}
			
		buf.erase(std::remove(buf.begin(), buf.end(), '\n'), buf.end());
		pos = buf.find(",");
		if (pos == string::npos)
			continue;
		sContractNo = buf.substr(0, pos);
		sACSymbol = buf.substr(pos + 1, string::npos);
		ContractTransInfo contractinfo;
		
		if (!sContractNo.empty() && !sACSymbol.empty())
		{
			//��ȡ������Լ�ĺ�Լ��
			int icount = g_pQuoteApi->GetContractData("", sContractNo.c_str(), contract, 1, false);
			if (icount)
			{
				underLay = g_pQuoteApi->GetContractUnderlay(sContractNo.c_str());
				if (underLay)
				{
					strcpy_s(contractinfo.SubContractNo, underLay->ContractNo);
				}
				else
				{
					continue;
				}
			}
		}
		TContractKey	TradeContract;
		CConvert::RawContractIdTypeToTradeContract(contractinfo.SubContractNo, TradeContract);
		
		//������ʢ��Լ����Ʒ�����·ݣ���AC��symbol����ƥ��
		int contractdate = atoi(TradeContract.ContractNo);
		int month = contractdate % 100;
		int year = (contractdate / 100);
		//֣�����ĺ�Լ�·ݱ�ʾ��ͬ���ֱ���
		if (strcmp(TradeContract.ExchangeNo,"ZCE") == 0)
		{
			year += 2010;
		}
		else
		{
			year += 2000;
		}
		contractinfo.months = MONTHSTR[month - 1];
		contractinfo.years = to_string(year);

		m_mapSymbolNameContractInfo.emplace(make_pair(sACSymbol, contractinfo));
	}
}
void ACAnalysisPresenter::UpdateKLineView(ACContract &contract)
{
	m_KLineView->UpdateKLineView(contract);
	char key[256] = { 0 };
	sprintf_s(key, "%s|%s", contract.commoditySymbol, contract.pattern);
	auto spiDataIter = m_KLineModel->m_TCDataMap.find(key);
	
	if (spiDataIter != m_KLineModel->m_TCDataMap.end())
	{
		m_KLineIntroduct->SetText(spiDataIter->second);
		InvalidateRect(m_KLineIntroduct->GetHwnd(), nullptr, false);
	}
}
const ACContract ACAnalysisPresenter::GetContract() 
{
	return m_KContract;
}
LRESULT ACAnalysisPresenter::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
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
		return PROCESSED;
	default:
		break;
	}
	return NOT_PROCESSED;
}

void ACAnalysisPresenter::OnHisQuote(const SContractNoType contractno, const SKLineTypeType klinetype, const SSessionIdType sessionid) 
{
	if (strcmp(m_KContract.SubContractNo, contractno) == 0) 
	{
		PostMessage(m_Hwnd, WM_ONQUOTE, (WPARAM)klinetype, (LPARAM)sessionid);	
	}	
}

void ACAnalysisPresenter::OnClickListItem(string exchangeNo)
{
	//������µĽ�����ʱ����������ȡ������ʹ��֮ǰ�洢��
	if (m_sCurExchangeNo != exchangeNo)
	{
		m_bChangeExchangeData = false;
		m_sCurExchangeNo = exchangeNo;
		auto iter = m_KLineModel->m_ExchangeNoACDataMap.find(m_sCurExchangeNo);
		if (iter != m_KLineModel->m_ExchangeNoACDataMap.end())
		{
			m_KLineModel->OnHttpInitReceive(iter->second);
		}
		else
		{
			GetTCData();
		}	
	}
}

void ACAnalysisPresenter::OnClickContractListItem(ContractInfoViewItemData contractItemData)
{
	m_KLineModel->SetContractInfoItem(contractItemData);
	char key[256] = { 0 };
	sprintf_s(key, "%s|%s", contractItemData.symbol, contractItemData.pattern);
	auto spiDataIter = m_KLineModel->m_TCDataMap.find(key);
	if (spiDataIter != m_KLineModel->m_TCDataMap.end())
	{
		m_KLineIntroduct->SetText(spiDataIter->second);
	}
}

void ACAnalysisPresenter::OnTabChange(int index)
{
	if (m_KLineModel->m_iTCTerm != index)
	{
		m_KLineModel->m_iTCTerm = index;
	}
}
//��Ҫ�����ط��������ݣ�
//1.��ʼ����
//2.ʮ��������һ��Timer��
//3.����л�TC���ڣ�
//4.����б��л�Ʒ��
void ACAnalysisPresenter::GetTCDataFromMap()
{
	m_KLineModel->InitDataFromTCMap();
	m_KLineModel->UpdateSubData(true);
	UpdateKLineView(m_KContract);
}
void ACAnalysisPresenter::GetTCData()
{
	AC_Service_Type exchangeno = {};
	strcpy_s(exchangeno, m_sCurExchangeNo.c_str());
	getKLineModel()->GetHttpData(exchangeno, AsynGetAll);
}
void ACAnalysisPresenter::GetTCDataSpi()
{
	UpdateView();
}
void ACAnalysisPresenter::UpdateView()
{
	m_KLineModel->UpdateSubData(true);
	getKLineModel()->UpdateTotalData();
}

