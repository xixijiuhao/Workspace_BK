#pragma once
enum SpotHedgeType
{
	shNone = -1,//��Ч
	shOrder,	//0�µ�
	shFuture,	//1�ڻ�
	shCom,		//2�ۺ�
	shSetting,	//3����
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


class SendOrderWnd;					//��ͨ�µ�
class SpecWnd;						//���
class AgreementWnd;					//��ͬ
class SpecPriceWnd;					//���۸�
class SettlePriceWnd;				//�����
class CashInOutWnd;					//�����
class InstantPositionWnd;			//��ʱ�ֲ�
class PositionWnd;					//�ֲ�
class OrderWnd;						//ί��
class MatchWnd;						//�ɽ�
class FundWnd;						//�ʽ�
class TaxWnd;						//˰��
class IntegrationWnd;				//�ۺ�
class PartAccountWnd;				//����
class AllAccountWnd;				//����
class ExposureWnd;					//����

class SystemFrame : public TIxFrm ,public ISpotSystemSpi
{
public:
	SystemFrame();
	~SystemFrame();

	string			get_key(const SmsCompanyNoType companyno, const SmsUserNoType userno);
	string			get_curkey();
	vector<SmsLoginRsp>		m_vecLogin;					//��¼
	SmsLoginRsp				m_curLogin;					//��ǰ�û�

	//��
	//std::mutex						m_muMatch;

	//��������
	UserMapType						m_mapUser;					//��������Ա
	AgreementMapMapType				m_map_mapAgreement;			//��ͬ
	AgreementRelMapType				m_mapAgreementRel;			//��ͬ��ϵ
	ManualCloseMapType				m_mapManualClose;			//ָ��ƽ��
	SpecMapType						m_mapSpec;					//���
	TaxSetType						m_setTax;					//˰��

	//vector<SmsSpecPriceDataRsp>		m_vecSpecPrice;				//���۸�
	SpecPriceMapType				m_mapSpecPrice;				//���۸�
	vector<SmsSettlePriceDataRsp>	m_vecSettlePrice;			//�����
	vector<SmsCashInoutDataRsp>		m_vecCashInout;				//�����
	vector<SmsPositionDataRsp>		m_vecPosition;				//�ֲ�
	vector<SmsOrderDataRsp>			m_vecOrder;					//ί��
	vector<SmsFundDataRsp>			m_vecFund;					//�ʽ�
	//vector<SmsMatchDataRsp>			m_vecMatch;					//�ɽ�
	map<string, SmsMatchDataRsp>		m_mapMatch;					//�ɽ�

public:
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	void SetWindow();
	void switch_index(const int main_index, const int sub_index = 0);
	std::vector<std::string> SplitString(const std::string & str, char cSplit);
	void StrTimeToSysTime(string str, SYSTEMTIME &time);

protected:
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);	//���ڴ������
	void OnCreate();
	void OnPaint();
	void OnSize();
	void InitMenuItem();
	void OnLButtonDown(const int x, const int y);
	void OnLButtonUp(const int x, const int y);
	void UpdateSwitchWindow();
	void OnNcDestory();									//�ͷ�
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
	MatchWnd*				m_matchWnd;					//�ɽ�
	InstantPositionWnd*		m_instantPositionWnd;
	PositionWnd*			m_positionWnd;
	FundWnd*				m_fundWnd;
	CashInOutWnd*			m_cashInoutWnd;
	SettlePriceWnd*			m_settleSpriceWnd;

	IntegrationWnd*			m_integrationWnd;
	PartAccountWnd*			m_partAccountWnd;
	AllAccountWnd*			m_allAccountWnd;
	ExposureWnd*			m_exposureWnd;				//����

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
	//��¼���Ӧ��
	virtual void __cdecl OnInitComplete(const SmsLoginRsp login) ;
	//���׶���
	virtual void __cdecl OnTradeDisconnect(const SmsLoginRsp login);
	//���Ӻ�ͬ
	virtual void __cdecl OnAddAgreement(SmsAgreementAddRsp* rsp);
	//ɾ����ͬ
	virtual void __cdecl OnDeleteAgreement(SmsAgreementDelRsp* rsp) ;
	//�޸ĺ�ͬ
	virtual void __cdecl OnModifyAgreement(SmsAgreementModRsp* rsp) ;
	//��ѯ��ͬ
	virtual void __cdecl OnQryAgreement(SmsAgreementQryRsp* rsp) {};

	//���Ӻ�ͬ��ϵ
	virtual void __cdecl OnAddAgreementRel(SmsAgreementRelAddRsp* rsp) ;
	//ɾ����ͬ��ϵ
	virtual void __cdecl OnDeleteAgreementRel(SmsAgreementRelDelRsp* rsp) ;
	//�޸ĺ�ͬ��ϵ
	virtual void __cdecl OnModifyAgreementRel(SmsAgreementRelModRsp* rsp) ;
	//��ѯ��ͬ��ϵ
	virtual void __cdecl OnQryAgreementRel(SmsAgreementRelQryRsp* rsp) {};

	//���ӹ��
	virtual void __cdecl OnAddSpec(SmsSpecAddRsp* rsp) ;
	//ɾ�����
	virtual void __cdecl OnDeleteSpec(SmsSpecDelRsp* rsp);
	//�޸Ĺ��
	virtual void __cdecl OnModifySpec(SmsSpecModRsp* rsp);
	//��ѯ���
	virtual void __cdecl OnQrySpec(SmsSpecQryRsp* rsp) {};

	//���ӹ��۸�
	virtual void __cdecl OnAddSpecPrice(SmsSpecPriceAddRsp* rsp) ;
	//ɾ�����۸�
	virtual void __cdecl OnDeleteSpecPrice(SmsSpecPriceDelRsp* rsp);
	//�޸Ĺ��۸�
	virtual void __cdecl OnModifySpecPrice(SmsSpecPriceModRsp* rsp) ;
	//��ѯ���۸�
	virtual void __cdecl OnQrySpecPrice(SmsSpecPriceQryRsp* rsp);
	virtual void __cdecl OnQrySpecPriceData(SmsSpecPriceDataRsp* rsp);

	//���ӽ����
	virtual void __cdecl OnAddSettlePrice(SmsSettlePriceAddRsp* rsp) ;
	//ɾ�������
	virtual void __cdecl OnDeleteSettlePrice(SmsSettlePriceDelRsp* rsp) ;
	//�޸Ľ����
	virtual void __cdecl OnModifySettlePrice(SmsSettlePriceModRsp* rsp) ;
	//��ѯ�����
	virtual void __cdecl OnQrySettlePriceData(SmsSettlePriceDataRsp* rsp);
	virtual void __cdecl OnQrySettlePrice(SmsSettlePriceQryRsp* rsp);

	//�����˹�ƽ��
	virtual void __cdecl OnAddManualClose(SmsManualCloseAddRsp* rsp);
	//ɾ���˹�ƽ��
	virtual void __cdecl OnDeleteManualClose(SmsManualCloseDelRsp* rsp) ;
	//�޸��˹�ƽ��
	virtual void __cdecl OnModifyManualClose(SmsManualCloseModRsp* rsp) {};
	//��ѯ�˹�ƽ��
	virtual void __cdecl OnQryManualClose(SmsManualCloseQryRsp* rsp) {};

	//���ӳ����
	virtual void __cdecl OnAddCashInout(SmsCashInoutAddRsp* rsp) ;
	//ɾ�������
	virtual void __cdecl OnDeleteCashInout(SmsCashInoutDelRsp* rsp) ;
	//�޸ĳ����
	virtual void __cdecl OnModifyCashInout(SmsCashInoutModRsp* rsp) ;
	//��ѯ�����
	virtual void __cdecl OnQryCashInoutData(SmsCashInoutDataRsp* rsp);
	virtual void __cdecl OnQryCashInout(SmsCashInoutQryRsp* rsp);

	//�����ʽ�
	virtual void __cdecl OnAddFund(SmsFundAddRsp* rsp) ;
	//ɾ���ʽ�
	virtual void __cdecl OnDeleteFund(SmsFundDelRsp* rsp) ;
	//�޸��ʽ�
	virtual void __cdecl OnModifyFund(SmsFundModRsp* rsp) ;
	//��ѯ�ʽ�
	virtual void __cdecl OnQryFundData(SmsFundDataRsp* rsp);
	virtual void __cdecl OnQryFund(SmsFundQryRsp* rsp);

	//���ӳֲ�
	virtual void __cdecl OnAddPosition(SmsPositionAddRsp* rsp) ;
	//ɾ���ֲ�
	virtual void __cdecl OnDeletePosition(SmsPositionDelRsp* rsp) ;
	//�޸ĳֲ�
	virtual void __cdecl OnModifyPosition(SmsPositionModRsp* rsp) ;
	//��ѯ�ֲ�
	virtual void __cdecl OnQryPositionData(SmsPositionDataRsp* rsp);
	virtual void __cdecl OnQryPosition(SmsPositionQryRsp* rsp);

	//����ί��
	virtual void __cdecl OnAddOrder(SmsOrderAddRsp* rsp) {};
	//ɾ��ί��
	virtual void __cdecl OnDeleteOrder(SmsOrderDelRsp* rsp) ;
	//�޸�ί��
	virtual void __cdecl OnModifyOrder(SmsOrderModRsp* rsp) ;
	//��ѯί��
	virtual void __cdecl OnQryOrderData(SmsOrderDataRsp* rsp);
	virtual void __cdecl OnQryOrder(SmsOrderQryRsp* rsp) ;

	//���ӳɽ�
	virtual void __cdecl OnAddMatch(SmsMatchAddRsp* rsp) ;
	//ɾ���ɽ�
	virtual void __cdecl OnDeleteMatch(SmsMatchDelRsp* rsp) ;
	//�޸ĳɽ�
	virtual void __cdecl OnModifyMatch(SmsMatchModRsp* rsp) ;
	//��ѯ�ɽ�
	virtual void __cdecl OnQryMatchData(SmsMatchDataRsp* rsp);
	virtual void __cdecl OnQryMatch(SmsMatchQryRsp* rsp) ;

	//����˰��
	virtual void __cdecl OnAddTax(SmsTaxAddRsp* rsp) ;
	//ɾ��˰��
	virtual void __cdecl OnDeleteTax(SmsTaxDelRsp* rsp) ;
	//�޸�˰��
	virtual void __cdecl OnModifyTax(SmsTaxModRsp* rsp) ;
	//��ѯ˰��
	virtual void __cdecl OnQryTax(SmsTaxQryRsp* rsp) {};
};
