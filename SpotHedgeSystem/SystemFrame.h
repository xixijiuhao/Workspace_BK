#pragma once
enum SpotHedgeType
{
	shNone = -1,//无效
	shOrder,	//0下单
	shFuture,	//1期货
	shCom,		//2综合
	shSetting,	//3设置
	shEnd		//end
};

typedef vector<SmsUserDataRsp>				UserVecType;
typedef map<string, UserVecType>			UserMapType;

//typedef vector<SmsAgreementDataRsp>			AgreementVecType;
//typedef map<string, AgreementVecType>		AgreementMapType;  
typedef map<string, SmsAgreementDataRsp>	AgreementMapType;
typedef map<string, AgreementMapType>		AgreementMapMapType;

typedef vector<SmsAgreementRelDataRsp>		AgreementRelVecType;
typedef map<string, AgreementRelVecType>	AgreementRelMapType;

typedef vector<SmsManualCloseDataRsp>		ManualCloseVecType;
typedef map<string, ManualCloseVecType>		ManualCloseMapType;

typedef map<string, SmsSpecDataRsp>			SpecMapType;

typedef set<SmsSpecPriceDataRsp>			SpecPriceSetType;
typedef map<string, SpecPriceSetType>		SpecPriceMapType;

typedef set<SmsTaxDataRsp>					TaxSetType;


class SendOrderWnd;					//普通下单
class SpecWnd;						//规格
class AgreementWnd;					//合同
class SpecPriceWnd;					//规格价格
class SettlePriceWnd;				//结算价
class CashInOutWnd;					//出入金
class InstantPositionWnd;			//即时持仓
class PositionWnd;					//持仓
class OrderWnd;						//委托
class MatchWnd;						//成交
class FundWnd;						//资金
class TaxWnd;						//税率
class IntegrationWnd;				//综合
class PartAccountWnd;				//分账
class AllAccountWnd;				//总账
class ExposureWnd;					//敞口

class SystemFrame : public TIxFrm ,public ISpotSystemSpi
{
public:
	SystemFrame();
	~SystemFrame();

	string			get_key(const SmsCompanyNoType companyno, const SmsUserNoType userno);
	string			get_curkey();
	vector<SmsLoginRsp>		m_vecLogin;					//登录
	SmsLoginRsp				m_curLogin;					//当前用户

	//锁
	//std::mutex						m_muMatch;

	//数据容器
	UserMapType						m_mapUser;					//下属交易员
	AgreementMapMapType				m_map_mapAgreement;			//合同
	AgreementRelMapType				m_mapAgreementRel;			//合同关系
	ManualCloseMapType				m_mapManualClose;			//指定平仓
	SpecMapType						m_mapSpec;					//规格
	TaxSetType						m_setTax;					//税率

	//vector<SmsSpecPriceDataRsp>		m_vecSpecPrice;				//规格价格
	SpecPriceMapType				m_mapSpecPrice;				//规格价格
	vector<SmsSettlePriceDataRsp>	m_vecSettlePrice;			//结算价
	vector<SmsCashInoutDataRsp>		m_vecCashInout;				//出入金
	vector<SmsPositionDataRsp>		m_vecPosition;				//持仓
	vector<SmsOrderDataRsp>			m_vecOrder;					//委托
	vector<SmsFundDataRsp>			m_vecFund;					//资金
	//vector<SmsMatchDataRsp>			m_vecMatch;					//成交
	map<string, SmsMatchDataRsp>		m_mapMatch;					//成交

public:
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	void SetWindow();
	void switch_index(const int main_index, const int sub_index = 0);
	std::vector<std::string> SplitString(const std::string & str, char cSplit);
	void StrTimeToSysTime(string str, SYSTEMTIME &time);

protected:
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);	//窗口处理过程
	void OnCreate();
	void OnPaint();
	void OnSize();
	void InitMenuItem();
	void OnLButtonDown(const int x, const int y);
	void OnLButtonUp(const int x, const int y);
	void UpdateSwitchWindow();
	void OnNcDestory();									//释放
	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnMouseLeave();
	void OnComboxSelect(WPARAM wParam, LPARAM lParam);
public:
	SendOrderWnd*			m_sendOrderWnd;

	AgreementWnd*			m_agreeWnd;
	SpecWnd*				m_specWnd;
	SpecPriceWnd*			m_specSpriceWnd;
	TaxWnd*					m_taxWnd;

	OrderWnd*				m_orderWnd;
	MatchWnd*				m_matchWnd;					//成交
	InstantPositionWnd*		m_instantPositionWnd;
	PositionWnd*			m_positionWnd;
	FundWnd*				m_fundWnd;
	CashInOutWnd*			m_cashInoutWnd;
	SettlePriceWnd*			m_settleSpriceWnd;

	IntegrationWnd*			m_integrationWnd;
	PartAccountWnd*			m_partAccountWnd;
	AllAccountWnd*			m_allAccountWnd;
	ExposureWnd*			m_exposureWnd;				//敞口

private:
	void init();

	void get_main_bar_rect(RECT & rect);
	void get_sub_bar_rect(RECT & rect);
	void get_dlg_rect(RECT & rect);

	void draw_main_tab(HDC dc);
	void draw_sub_tab(HDC dc);
	void draw_login_tab(HDC dc);

	bool pt_in_main_bar(const POINT & pt, __out int & ind);
	bool pt_in_sub_bar(const POINT & pt, __out int & ind);
	bool pt_in_login(const POINT & pt);

	HWND	m_current_dlg;

	int m_frame_weight;
	int m_main_bar_height;
	int m_main_bar_width;
	int m_sub_bar_height;
	int m_sub_bar_width;
	int m_dlg_gap;
	
	int m_sub_text_left;

	int m_current_main_index;
	int m_current_sub_index;

	COLORREF m_main_bar_back_color;
	COLORREF m_main_bar_text_color;
	COLORREF m_main_bar_back_sel_color;
	COLORREF m_main_bar_text_sel_color;

	COLORREF m_sub_bar_back_color;
	COLORREF m_sub_bar_text_color;
	COLORREF m_sub_bar_back_sel_color;
	COLORREF m_sub_bar_text_sel_color;

	LOGFONT m_main_bar_font;
	LOGFONT m_sub_bar_font;

	struct TSub
	{
		HWND	sub_hwnd;
		int		index;
	};

	typedef pair<wstring, TSub>				sub_pair;
	typedef vector<sub_pair>				sub_vector;
	typedef pair<wstring, sub_vector>		main_pair;
	typedef vector<main_pair>				main_vector;
	main_vector								m_main_bars;

	typedef vector<int>						sbu_index_of_main_vector;
	sbu_index_of_main_vector				m_sub_indexs;

	bool									m_bMouseTrack;
	TLoginPopWnd							m_loginWnd;
	RECT									m_rcLogin;
	RECT									m_rcUserNo;
	bool									m_bInLoginRect;
private:
	void			Init_User(const SmsCompanyNoType companyno, const SmsUserNoType userno);
	void			Init_Agreement(const SmsCompanyNoType companyno, const SmsUserNoType userno);
	void			Init_AgreementRel(const SmsCompanyNoType companyno, const SmsUserNoType userno);
	void			Init_ManualClose(const SmsCompanyNoType companyno, const SmsUserNoType userno);
	void			Init_Spec(const SmsCompanyNoType companyno, const SmsUserNoType userno);
	void			Init_Tax(const SmsCompanyNoType companyno, const SmsUserNoType userno);
public:
	//登录完成应答
	virtual void __cdecl OnInitComplete(const SmsLoginRsp login) ;
	//交易断线
	virtual void __cdecl OnTradeDisconnect(const SmsLoginRsp login);
	//增加合同
	virtual void __cdecl OnAddAgreement(SmsAgreementAddRsp* rsp);
	//删除合同
	virtual void __cdecl OnDeleteAgreement(SmsAgreementDelRsp* rsp) ;
	//修改合同
	virtual void __cdecl OnModifyAgreement(SmsAgreementModRsp* rsp) ;
	//查询合同
	virtual void __cdecl OnQryAgreement(SmsAgreementQryRsp* rsp) {};

	//增加合同关系
	virtual void __cdecl OnAddAgreementRel(SmsAgreementRelAddRsp* rsp) ;
	//删除合同关系
	virtual void __cdecl OnDeleteAgreementRel(SmsAgreementRelDelRsp* rsp) ;
	//修改合同关系
	virtual void __cdecl OnModifyAgreementRel(SmsAgreementRelModRsp* rsp) ;
	//查询合同关系
	virtual void __cdecl OnQryAgreementRel(SmsAgreementRelQryRsp* rsp) {};

	//增加规格
	virtual void __cdecl OnAddSpec(SmsSpecAddRsp* rsp) ;
	//删除规格
	virtual void __cdecl OnDeleteSpec(SmsSpecDelRsp* rsp);
	//修改规格
	virtual void __cdecl OnModifySpec(SmsSpecModRsp* rsp);
	//查询规格
	virtual void __cdecl OnQrySpec(SmsSpecQryRsp* rsp) {};

	//增加规格价格
	virtual void __cdecl OnAddSpecPrice(SmsSpecPriceAddRsp* rsp) ;
	//删除规格价格
	virtual void __cdecl OnDeleteSpecPrice(SmsSpecPriceDelRsp* rsp);
	//修改规格价格
	virtual void __cdecl OnModifySpecPrice(SmsSpecPriceModRsp* rsp) ;
	//查询规格价格
	virtual void __cdecl OnQrySpecPrice(SmsSpecPriceQryRsp* rsp);
	virtual void __cdecl OnQrySpecPriceData(SmsSpecPriceDataRsp* rsp);

	//增加结算价
	virtual void __cdecl OnAddSettlePrice(SmsSettlePriceAddRsp* rsp) ;
	//删除结算价
	virtual void __cdecl OnDeleteSettlePrice(SmsSettlePriceDelRsp* rsp) ;
	//修改结算价
	virtual void __cdecl OnModifySettlePrice(SmsSettlePriceModRsp* rsp) ;
	//查询结算价
	virtual void __cdecl OnQrySettlePriceData(SmsSettlePriceDataRsp* rsp);
	virtual void __cdecl OnQrySettlePrice(SmsSettlePriceQryRsp* rsp);

	//增加人工平仓
	virtual void __cdecl OnAddManualClose(SmsManualCloseAddRsp* rsp);
	//删除人工平仓
	virtual void __cdecl OnDeleteManualClose(SmsManualCloseDelRsp* rsp) ;
	//修改人工平仓
	virtual void __cdecl OnModifyManualClose(SmsManualCloseModRsp* rsp) {};
	//查询人工平仓
	virtual void __cdecl OnQryManualClose(SmsManualCloseQryRsp* rsp) {};

	//增加出入金
	virtual void __cdecl OnAddCashInout(SmsCashInoutAddRsp* rsp) ;
	//删除出入金
	virtual void __cdecl OnDeleteCashInout(SmsCashInoutDelRsp* rsp) ;
	//修改出入金
	virtual void __cdecl OnModifyCashInout(SmsCashInoutModRsp* rsp) ;
	//查询出入金
	virtual void __cdecl OnQryCashInoutData(SmsCashInoutDataRsp* rsp);
	virtual void __cdecl OnQryCashInout(SmsCashInoutQryRsp* rsp);

	//增加资金
	virtual void __cdecl OnAddFund(SmsFundAddRsp* rsp) ;
	//删除资金
	virtual void __cdecl OnDeleteFund(SmsFundDelRsp* rsp) ;
	//修改资金
	virtual void __cdecl OnModifyFund(SmsFundModRsp* rsp) ;
	//查询资金
	virtual void __cdecl OnQryFundData(SmsFundDataRsp* rsp);
	virtual void __cdecl OnQryFund(SmsFundQryRsp* rsp);

	//增加持仓
	virtual void __cdecl OnAddPosition(SmsPositionAddRsp* rsp) ;
	//删除持仓
	virtual void __cdecl OnDeletePosition(SmsPositionDelRsp* rsp) ;
	//修改持仓
	virtual void __cdecl OnModifyPosition(SmsPositionModRsp* rsp) ;
	//查询持仓
	virtual void __cdecl OnQryPositionData(SmsPositionDataRsp* rsp);
	virtual void __cdecl OnQryPosition(SmsPositionQryRsp* rsp);

	//增加委托
	virtual void __cdecl OnAddOrder(SmsOrderAddRsp* rsp) {};
	//删除委托
	virtual void __cdecl OnDeleteOrder(SmsOrderDelRsp* rsp) ;
	//修改委托
	virtual void __cdecl OnModifyOrder(SmsOrderModRsp* rsp) ;
	//查询委托
	virtual void __cdecl OnQryOrderData(SmsOrderDataRsp* rsp);
	virtual void __cdecl OnQryOrder(SmsOrderQryRsp* rsp) ;

	//增加成交
	virtual void __cdecl OnAddMatch(SmsMatchAddRsp* rsp) ;
	//删除成交
	virtual void __cdecl OnDeleteMatch(SmsMatchDelRsp* rsp) ;
	//修改成交
	virtual void __cdecl OnModifyMatch(SmsMatchModRsp* rsp) ;
	//查询成交
	virtual void __cdecl OnQryMatchData(SmsMatchDataRsp* rsp);
	virtual void __cdecl OnQryMatch(SmsMatchQryRsp* rsp) ;

	//增加税率
	virtual void __cdecl OnAddTax(SmsTaxAddRsp* rsp) ;
	//删除税率
	virtual void __cdecl OnDeleteTax(SmsTaxDelRsp* rsp) ;
	//修改税率
	virtual void __cdecl OnModifyTax(SmsTaxModRsp* rsp) ;
	//查询税率
	virtual void __cdecl OnQryTax(SmsTaxQryRsp* rsp) {};
};
