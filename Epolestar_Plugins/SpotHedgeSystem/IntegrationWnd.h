#pragma once

typedef struct HedgeInfo
{
	SmsAgreementNoType          OpenAgreementNo;            //平仓合同编号
	SmsAgreementNoType          CloseAgreementNo;           //平仓合同编号
	SmsMatchNoType              CloseNo;                    //平仓编号
	SmsMatchNoType              OpenNo;                     //开仓编号
	SmsQtyType                  CloseQty;                   //平仓数量
	HedgeInfo()
	{
		memset(this, 0, sizeof(HedgeInfo));
	}
} HedgeInfo;

class AccountBillListWnd;
class IntegrationWnd :public TIxFrm
{
public:
	IntegrationWnd();
	~IntegrationWnd();
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	void Register(SystemFrame* pdata);
	CxCombox			m_cbAgree;
	CxCombox			m_cbUserno;

	map<string, map<string, SmsMatchDataRsp>>											m_map_mAgreeMatch;		//<合同号，<成交号，成交数据>>
	map<string, SmsAgreementDataRsp>													m_map_Agree;			//<合同号，合同数据>
	map<SmsAgreementDataRsp, set<SmsAgreementDataRsp>>									m_map_sAgreeRel;		//<开仓数据，<平仓数据>>
	SpecPriceMapType																	m_map_specPrice;		//现货价格

	HANDLE										m_hQryMatch;
	HANDLE										m_hQrySpecPrice;
protected:
	enum ControlID
	{
		ID_Begin,

		ID_LabBeginDate,
		ID_CbtBeginDate,
		ID_LabEndDate,
		ID_CbtEndDate,
		ID_LabAgree,
		ID_CbAgree,
		ID_LabUserno,
		ID_CbUserno,

		ID_BtnQry,
		ID_BtnSave,

		ID_EditAll,

		ID_End
	};
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnCreate();
	void OnPaint();
	void OnCommand(WPARAM wParam, LPARAM lParam);
	//void OnNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnCtlColor(WPARAM wParam, LPARAM lParam);
	void OnNcDestroy();
	void OnDestroy();
	void OnSize(WPARAM wParam, LPARAM lParam);
	void OnBtnClickDown(WPARAM wParam, LPARAM lParam);

	void OnBtnQry();
	void OnBtnSave();

	void GeneratedBill();
	void GeneratedListBill();

	void GeneratedItem(SmsAgreementDataRsp mainAgree, SmsAgreementDataRsp minorAgree, vector<HedgeInfo>& v_hedgeInfo, bool isfirst = false, int close_remain =0, bool isfirstmain = false);
	void GeneratedListItem(SmsAgreementDataRsp mainAgree, SmsAgreementDataRsp minorAgree, vector<HedgeInfo>& v_hedgeInfo, bool isfirst = false, int close_remain = 0, bool isfirstmain = false);
	
	void GeneratedSelfOffsetItem(SmsAgreementDataRsp mainAgree, vector<HedgeInfo>& v_hedgeInfo, bool isfirst = false, bool isfirstmain =false);
	void GeneratedSelfOffsetListItem(SmsAgreementDataRsp mainAgree, vector<HedgeInfo>& v_hedgeInfo, bool isfirst = false, bool isfirstmain = false);

	void CalculateSpotOpen(SmsAgreementDataRsp mainAgree, bool isfirst = false, bool isfirstmain = false);
	void CalculateSpotOpenList(SmsAgreementDataRsp mainAgree, bool isfirst = false, bool isfirstmain = false);

	void FindMinorAgree(string maingreeno,set<SmsAgreementDataRsp>& set_minoragree);
	std::vector<std::string> SplitString(const std::string & str, char cSplit);
	std::vector<std::string> SplitString(const std::string & str, const string sSplit);
	std::vector<std::string> SplitStringWithoutNone(const std::string & str, char cSplit);
	void TrimString(string &str);
	int	 DateStrToInt(const std::string & str);
	bool SaveToExcel(wstring filename);

	void InitSplit();

private:
	SystemFrame * m_System;

	HWND				m_beginTimePick;
	HWND				m_endTimePick;

	TStatic				m_labBeginDate;
	CxComboxTime		m_cbtBeginDate;
	TStatic				m_labEndDate;
	CxComboxTime		m_cbtEndDate;

	TStatic				m_labAgree;

	TStatic				m_labUserno;

	TStaticButton		m_btnQry;
	TStaticButton		m_btnSave;

	HINSTANCE			m_hInstance;
	CxRichEdit			m_richeditAll;									//对账单
	AccountBillListWnd	m_listAll;										//对账表

private:
	TRowInfoContainer				m_RowinfoContainer;			//账单信息
	double	m_spotAllProfit;		//现货损益
	double  m_allFee;				//手续费合计
	double  m_matchAllProfit;		//期货盈亏

	double	m_tradeDot;				//每手乘数
	int		m_minorAgreeQty;		//次要合同数量总计
	double	m_minorAgreeTotal;		//次要合同价值总计
	double  m_spotTotalProfit;		//损益合计

	int		m_futureQty ;			//期货数量合计
	int     m_futureRemainQty;		//未对冲期货合计
	double  m_openMatchTotal;		//开仓价格合计
	double  m_closeMatchTotal;		//平仓价格合计
	double  m_totalFee;				//手续费合计
	double  m_matchTotalProfit;		//期货盈亏

	string row_title_start;
	string row_title_middle1;
	string row_title_middle2;
	string row_text_start;
	string row_text_middle_l;
	string row_text_middle_r;
	string row_title_end;
	string row_text_sumtop;
	string row_text_sumbottom;
	string row_text_totalsumtop;

	//string row_split;
	//string row_dblv;
	//string row_mid_l;
	//string row_mid_r;
};



////====================================================指定平仓==============================================================
//for (auto it_manual : m_System->m_mapManualClose[key])
//{
//	if (strcmp(it_manual.CloseAgreementNo,it_close.AgreementNo) == 0)
//	{
//		HedgeInfo tempinfo;
//		strncpy_s(tempinfo.OpenAgreementNo, it_open.first.AgreementNo);
//		strncpy_s(tempinfo.CloseAgreementNo, it_close.AgreementNo);
//		strncpy_s(tempinfo.OpenNo, it_manual.OpenNo);
//		strncpy_s(tempinfo.CloseNo, it_manual.CloseNo);
//		tempinfo.CloseQty = it_manual.CloseQty;
//		v_matchNum.push_back(tempinfo);
//	}
//}
////根据开仓合同号，平仓合同号，消除对应的成交数量
//for (auto it_reduceNum : v_matchNum)
//{
//	//找开仓合同成交
//	auto it_openAgree = m_map_mAgreeMatch.find(it_open.first.AgreementNo);
//	if (it_openAgree != m_map_mAgreeMatch.end())
//	{
//		m_map_mAgreeMatch[it_open.first.AgreementNo][it_reduceNum.OpenNo].MatchQty -= it_reduceNum.CloseQty;
//	}
//	//找平仓合同成交
//	auto it_closeAgree = m_map_mAgreeMatch.find(it_close.AgreementNo);
//	if (it_closeAgree != m_map_mAgreeMatch.end())
//	{
//		m_map_mAgreeMatch[it_close.AgreementNo][it_reduceNum.CloseNo].MatchQty -= it_reduceNum.CloseQty;
//	}
//}
////=======================================================================================================================
////=======================================================================================================================
//string userno("");
//string opendate("");
//string openagree("");
//double openprice = 0.0;
//
////平仓合同现货利润
//double tax = 1.16;
//if (!m_System->m_setTax.empty())
//{
//	auto it = m_System->m_setTax.end();
//	tax = (*(--it)).Tax;
//}
//double spotprofit = (closeAgree.SpotPrice - openAgree.SpotPrice)*closeAgree.SpotQty / tax;
////每个平仓合同计算合计
//m_closeAgreeQty += closeAgree.SpotQty;
//m_closeAgreeTotal += closeAgree.SpotQty * closeAgree.SpotPrice;
//m_spotTotalProfit += spotprofit;
//
//if (v_hedgeInfo.size() == 0)
//{
//	if (isfirst)
//	{
//		userno = openAgree.UserNo;
//		opendate = openAgree.SignDate;
//		openagree = openAgree.AgreementNo;
//		if (openAgree.AgreementType == SMS_AGREEMENT_PURCHASE)
//			openagree.append("(采购)");
//		else if (openAgree.AgreementType == SMS_AGREEMENT_SALE)
//			openagree.append("(销售)");
//		openprice = openAgree.SpotPrice;
//
//		char temp_txt[1024] = { 0 };
//		sprintf_s(temp_txt, "%-6s|%-20s|%-10s|%-20s|%10.2f|%8d||%-10s|%-20s|%8d|%10.2f|%10.2f||%-20s|%10.2f|%10.2f|%8d|%10.2f|%10.2f|%-12s|",
//			"", userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, closeAgree.SpotQty,
//			closeAgree.SignDate, closeAgree.AgreementNo, closeAgree.SpotQty, closeAgree.SpotPrice, spotprofit,
//			"", 0.0, 0.0, 0, 0.0, 0.0, "");
//		m_richeditAll.AddString(row_split);
//		m_richeditAll.AddString(temp_txt);
//	}
//	else
//	{
//		char temp_txt[1024] = { 0 };
//		sprintf_s(temp_txt, "%-6s|%-20s|%-10s|%-20s|%10s|%8d||%-10s|%-20s|%8d|%10.2f|%10.2f||%-20s|%10.2f|%10.2f|%8d|%10.2f|%10.2f|%-12s|",
//			"", "", "", "", "", closeAgree.SpotQty,
//			closeAgree.SignDate, closeAgree.AgreementNo, closeAgree.SpotQty, closeAgree.SpotPrice, spotprofit,
//			"", 0.0, 0.0, 0, 0.0, 0.0, "");
//		m_richeditAll.AddString(row_mid);
//		m_richeditAll.AddString(temp_txt);
//	}
//	return;
//}
//
//for (int i = 0; i < v_hedgeInfo.size(); i++)
//{
//	SmsMatchDataRsp openmatch = { 0 };
//	auto it_open_agree = m_map_mAgreeMatch.find(v_hedgeInfo.at(i).OpenAgreementNo);
//	if (it_open_agree != m_map_mAgreeMatch.end())
//	{
//		auto it_open_match = m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo].find(v_hedgeInfo.at(i).OpenNo);
//		if (it_open_match != m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo].end())
//		{
//			memcpy(&openmatch, &m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo][v_hedgeInfo.at(i).OpenNo], sizeof(SmsMatchDataRsp));
//		}
//	}
//
//	SmsMatchDataRsp closematch = { 0 };
//	auto it_close_agree = m_map_mAgreeMatch.find(v_hedgeInfo.at(i).CloseAgreementNo);
//	if (it_close_agree != m_map_mAgreeMatch.end())
//	{
//		auto it_close_match = m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo].find(v_hedgeInfo.at(i).CloseNo);
//		if (it_close_match != m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo].end())
//		{
//			memcpy(&closematch, &m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo][v_hedgeInfo.at(i).CloseNo], sizeof(SmsMatchDataRsp));
//		}
//	}
//
//	//每手乘数
//	if (m_tradeDot == 0.0)
//	{
//		SContract* commod[1] = { 0 };
//		g_pStarApi->GetContractData("", openmatch.ContractNo, commod, 1, false);
//		if (commod[0] && commod[0]->Commodity)
//			m_tradeDot = commod[0]->Commodity->TradeDot;
//	}
//
//	//计算一笔对冲 手续费 期货利润
//	double fee = v_hedgeInfo.at(i).CloseQty*(openmatch.MatchFee + closematch.MatchFee);
//	double futureprofit = (openmatch.MatchPrice - closematch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;
//
//	//每笔对冲计算合计
//	m_futureQty += v_hedgeInfo.at(i).CloseQty;
//	m_openMatchTotal += openmatch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
//	m_closeMatchTotal += closematch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
//	m_totalFee += fee;
//	m_matchTotalProfit += futureprofit;
//
//	//行情合约转交易合约
//	string scode = openmatch.ContractNo;
//	string tcode("");
//	CConvert::SContractNoToTContractNo(scode, tcode);
//
//	if (i == 0 && isfirst)
//	{
//		userno = openAgree.UserNo;
//		opendate = openAgree.SignDate;
//		openagree = openAgree.AgreementNo;
//		if (openAgree.AgreementType == SMS_AGREEMENT_PURCHASE)
//			openagree.append("(采购)");
//		else if (openAgree.AgreementType == SMS_AGREEMENT_SALE)
//			openagree.append("(销售)");
//		openprice = openAgree.SpotPrice;
//
//		char temp_txt[1024] = { 0 };
//		sprintf_s(temp_txt, "%-6s|%-20s|%-10s|%-20s|%10.2f|%8d||%-10s|%-20s|%8d|%10.2f|%10.2f||%-20s|%10.2f|%10.2f|%8d|%10.2f|%10.2f|%-12s|",
//			"", userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, closeAgree.SpotQty,
//			closeAgree.SignDate, closeAgree.AgreementNo, closeAgree.SpotQty, closeAgree.SpotPrice, spotprofit,
//			tcode.c_str()/*openmatch.ContractNo*/, openmatch.MatchPrice, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
//		m_richeditAll.AddString(row_split);
//		m_richeditAll.AddString(temp_txt);
//	}
//	else if (i == 0)
//	{
//		//double spotprofit = (closeAgree.SpotPrice - openAgree.SpotPrice)*closeAgree.SpotQty;
//		//double fee = v_hedgeInfo.at(i).CloseQty*(openmatch.MatchFee + closematch.MatchFee);
//		//double futureprofit = (openmatch.MatchPrice - closematch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;
//
//		char temp_txt[1024] = { 0 };
//		sprintf_s(temp_txt, "%-6s|%-20s|%-10s|%-20s|%10s|%8d||%-10s|%-20s|%8d|%10.2f|%10.2f||%-20s|%10.2f|%10.2f|%8d|%10.2f|%10.2f|%-12s|",
//			"", "", "", "", "", closeAgree.SpotQty,
//			closeAgree.SignDate, closeAgree.AgreementNo, closeAgree.SpotQty, closeAgree.SpotPrice, spotprofit,
//			tcode.c_str(), openmatch.MatchPrice, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
//		m_richeditAll.AddString(row_mid);
//		m_richeditAll.AddString(temp_txt);
//	}
//	else
//	{
//		//double spotprofit = (closeAgree.SpotPrice - openAgree.SpotPrice)*closeAgree.SpotQty;
//		//double fee = v_hedgeInfo.at(i).CloseQty*(openmatch.MatchFee + closematch.MatchFee);
//		//double futureprofit = (openmatch.MatchPrice - closematch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;
//		char temp_txt[1024] = { 0 };
//		sprintf_s(temp_txt, "%-6s|%-20s|%-10s|%-20s|%10s|%8s||%-10s|%-20s|%8s|%10s|%10s||%-20s|%10.2f|%10.2f|%8d|%10.2f|%10.2f|%-12s|",
//			"", "", "", "", "", "",
//			"", "", "", "", "",
//			tcode.c_str(), openmatch.MatchPrice, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
//		m_richeditAll.AddString(temp_txt);
//	}
//}
////=======================================================================================================================