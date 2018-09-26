#ifndef _TDEFINE_H
#define _TDEFINE_H

#define SSWM_SYSTEM_SELSIGN					WM_USER + 97
#define SSWM_SYSTEM_NOTIFY					WM_USER + 98
#define SSWM_TABBUTTON_SELECT_CHANAGE		WM_USER + 99
#define SSWM_TAB_SELECT_CHANAGE				WM_USER + 100
#define SSWM_TRADE_DATA_USER_PUSH			WM_USER + 101
#define SSWM_TRADE_DATA_ORDER_PUSH			WM_USER + 102
#define SSWM_TRADE_DATA_MATCH_PUSH			WM_USER + 103
#define SSWM_TRADE_DATA_MONEY_PUSH			WM_USER + 104

#define SSWM_TRADE_DATA_LIQUIDATE_PUSH		WM_USER + 105
//#define SSWM_TAB_SWITCH_FOCUS				WM_USER + 107
#define SSWM_SET_EDIT_CLICKDOWN				WM_USER + 108
//#define SSWM_STATIC_BUTTON_CLICKDOWN		WM_USER + 109
//#define SSWM_COMBOX_SELECT					WM_USER + 110

#define SSWM_TRADE_DATA_POS_PUSH			WM_USER + 111
#define SSWM_TRADE_DATA_TOTALPOS_PUSH		WM_USER + 112
#define SSWM_DATA_INFO_SHOW_MENU			WM_USER + 113
#define SSWM_GRIDCELL_CHECKBOX				WM_USER + 114
#define SSWM_GRIDCELL_EDIT					WM_USER + 115
#define SSWM_GRIDCELL_COMBOX				WM_USER + 116

#define SSWM_CONFIG_GRIDLIST				WM_USER + 117
#define SSWM_GRIDCTRL_COL_WIDTH				WM_USER + 118
#define SSWM_REFRESHDATA_HOLD				WM_USER + 119
#define SSWM_HISTORY_ORDER_PROC_QUERY		WM_USER + 120
#define SSWM_HISTORY_ORDER_PROC_PUSH		WM_USER + 121
#define SSWM_NOTIFY_SHOW_PROCESS			WM_USER + 122
#define SSWM_GRIDCTRL_AUTO_COL_WIDTH		WM_USER + 124
#define SSWM_GRIDCTRL_SWAP_COL				WM_USER + 125
//#define SSWM_COMBOX_CHECK_CHANAGE			WM_USER + 126
#define SSWM_ENQUIRY_INFO_PUSH				WM_USER + 127
//#define SSWM_USER_PROXY_PUSH				WM_USER + 127
#define SSWM_HISQUOTE_DATA_PUSH				WM_USER + 128
#define SSWM_HISQUOTE_REV_END				WM_USER + 129
#define SSWM_EXCHANGE_STATUS_REFRESH		WM_USER + 130
#define SSWM_STOP_LOSS_PUSH					WM_USER + 131
#define SSWM_GRIDCTRL_HOVER					WM_USER + 132
#define SSWM_CALENDAR_CHANGE				WM_USER + 133
#define SSWM_HISMATCH_PUSH					WM_USER + 134
#define SSWM_AUTO_HSCROLL					WM_USER + 135
#define SSWM_HISENTRUST_PUSH				WM_USER + 136
#define SSWM_GRIDCTRL_EXPAND				WM_USER + 137
#define SSWM_GRIDLIST_EXPAND_Click			WM_USER + 138
#define SSWM_GRIDLIST_EXPAND_HOVER			WM_USER + 139
#define SSWM_BILLDATE_PUSH					WM_USER + 140
#define SSWM_GRIDLIST_EXPAND_CloseAll		WM_USER + 141
#define SSWM_COMBOX_LIST_INPUT_DOWN			WM_USER + 142
#define SSWM_COMBOX_LIST_INPUT_UP			WM_USER + 143
#define SSWM_USERPROXY_PUSH					WM_USER + 144
#define SSWM_STRATEGY_PUSH					WM_USER + 145
#define SSWM_SPREAD_PUSH					WM_USER + 146
#define SSWM_REFRESH_ALL_DATA				WM_USER + 147
#define SSWM_HISCASH_PUSH					WM_USER + 148
#define SSWM_TABBUTTON_SWAPNODE				WM_USER + 149
#define SSWM_GRIDCTRL_EXPAND_CHECK			WM_USER + 150
#define SSWM_TRADE_DATA_BUILDPOS_PUSH		WM_USER + 151

#define SSWM_UPDATEBANKINFO					WM_USER + 152
#define SSWM_TRANSFER_PUSH					WM_USER + 153
#define SSWM_BANK_BANLANCE_QRY				WM_USER + 154
#define SSWM_FUTURE_BANLANCE_QRY			WM_USER + 155
#define SSWM_MONEY_BANLANCE_QRY				WM_USER + 156
#define SSWM_TRANSFER_ERROR					WM_USER + 157
#define SSWM_SET_EDITBITMAP_CLICKDOWN		WM_USER + 158

#define SSWM_TRADE_MATCH_SUB_PUSH			WM_USER + 159
#define SSWM_TRADE_MATCH_TOTAL_PUSH			WM_USER + 160
#define SSWM_CHANGE_PASSWORD				WM_USER + 161
#define SSWM_TRADE_DATA_MSGQRY_PUSH			WM_USER + 162
#define SSWM_SIGN_GRID_SORT					WM_USER + 163
#define SSWM_GRID_MINIEDIT_MODITY			WM_USER + 164
#define SSWM_GRID_CELLEDIT_WHEEL			WM_USER + 165
#define SSWM_FORMINFO_FAST_STATE			WM_USER + 166


#define DOUBLE_MINI				(1e-10)
//
typedef struct tagFilter
{
	string			szaccount;			//资金账号
	char			szlocal[50];		//本地号
	char			szoperate[50];		//操作人
	char			szsytem[50];		//系统号
	char			szprice[50];		//价格
	string			szuser;				//会员
	string			szstate;			//状态
	string			szfrom;				//来源
	string			szmarket;			//市场
	string			sztype;				//品种
	char			szcontract[50];		//合约
	string			szholdcontract;		//持仓合约
	string			sztrade;			//买卖
	string			szliquidate;		//强平
	string			szcash;				//币种
	char			szremark[50];		//备注
	string			szvalidtype;		//有效时段
	string			szaddone;			//T+1

	tagFilter()
	{
		szaccount = "";
		szuser = "";
		szstate = "";
		szfrom = "";
		sztype = "";
		sztrade = "";
		szliquidate = "";
		szcash = "";
		memset(szprice, 0, 50);
		memset(szlocal, 0, 50);
		memset(szoperate, 0, 50);
		memset(szsytem, 0, 50);
		memset(szcontract, 0, 50);
		memset(szremark, 0, 50);
	}
	bool operator ==(tagFilter& stfilter)
	{
		if ( szaccount == stfilter.szaccount
			&& strcmp(szlocal,stfilter.szlocal)==0
			&& strcmp(szoperate,stfilter.szoperate)==0
			&& strcmp(szsytem,stfilter.szsytem)==0
			&& strcmp(szprice,stfilter.szprice)==0
			&& szuser == stfilter.szuser
			&& szstate == stfilter.szstate
			&& szfrom == stfilter.szfrom
			&& szmarket == stfilter.szmarket
			&& sztype ==  stfilter.sztype
			&& strcmp(szcontract,stfilter.szcontract)==0
			&& sztrade == stfilter.sztrade
			&& szliquidate == stfilter.szliquidate)
			return true;

		return false;
	}
}Filter;

enum SortSign
{
	Sort_Normal = 0,
	Sort_Upper,
	Sort_Down
};

typedef struct tagSortInfo
{
	int			nsortkey;
	bool			bupper;
	int			nsortcol;
	int			npresortcol;
	tagSortInfo()
	{
		nsortkey = -1;
		bupper = false;
		nsortcol = -1;
		npresortcol = -1;
	}
}SortInfo;

typedef struct tagConfigData
{
	BOOL			bshow;
	std::string		szcol;
	std::string		sztype;
	int				nrealindex;
	int				ncolwidth;
	int				nformat;//left 0 center 1 right 2
	bool			bignore;
	bool			bauthor;

	tagConfigData()
	{
		bshow = true;
		szcol = "";
		sztype = "";
		nrealindex = 0;
		ncolwidth = 0;
		nformat = 0;
		bignore = false;
		bauthor = true;
	}

	bool operator==(const tagConfigData& stdata)
	{
		if (nrealindex == stdata.nrealindex)
			return true;
		return false;
	}
}ConfigData;

enum SortFormKey
{
	FormKey_OrderID = 0,//订单号
	FormKey_StreamId,
	FormKey_OrderNo,//流号
	FormKey_LocalNo,//本地号
	FormKey_Operator,//操作人
	FormKey_SystemNo,//系统号
	FormKey_UpperNo,//会员
	FormKey_State,//状态
	FormKey_Source,//来源
	FormKey_Market,//市场
	FormKey_Breed,//品种
	FormKey_Contract,//合约
	FormKey_BS,//买卖
	FormKey_RiskOrder,//强平
	FormKey_OrderQty,//委托价格
	FormKey_OrderPrice,
	FormKey_MatchQty,//成交量
	FormKey_MatchPrice,//成交价
	FormKey_TriggerPrice, //触发价
	FormKey_SettleDate//结算日期
};
//成交查询
enum SortDealKey
{
	DealKey_DealNo = 0,
	DealKey_Account,
	DealKey_Market,
	DealKey_Name,
	DealKey_Contract,//
	DealKey_Trade,
	DealKey_Offset,//开平
	DealKey_MatchPrice,
	DealKey_MatchQty,
	DealKey_OrderPrice,
	DealKey_Commodity,//委托合约
	DealKey_MatchFee,
	DealKey_Premium,//权利金
	DealKey_MatchDate,
	DealKey_MatchTime,
	DealKey_SystemNo,
	DealKey_UserNo,
	DealKey_MatchWay,//成交来源
	DealKey_OrderType,//定单类型
	DealKey_CommodityType,//合约类型
	DealKey_UpperMatchDate,
	DealKey_UpperMatchTime,
	DealKey_AddOne,//T+1
	DealKey_OrderNo,
	DealKey_MatchNo,
	DealKey_InsertNo,
	DealKey_Cash,
	DealKey_CoverProfit,
	DealKey_StreamID,
	DealKey_SettleDate
};

enum SortHoldKey
{
	HoldKey_PositionPrice = 0,
	HoldKey_UserNo,
	HoldKey_UpperNo,
	HoldKey_ExchangeNo,
	HoldKey_CommodityName,
	HoldKey_RelateContractNo,
	HoldKey_Direct,
	HoldKey_PositionQty,
	HoldKey_ProfitCalcPrice,
	HoldKey_FloatProfit,
	HoldKey_FloatProfitTBT,
	HoldKey_DepositCalcPrice,
	HoldKey_Deposit,
	HoldKey_KeepDeposit,
	HoldKey_Delta,
	HoldKey_Gamma,
	HoldKey_MarketValue,
	HoldKey_DepositUpdateTime,
	HoldKey_CommodityType,
	HoldKey_Hedge,
	HoldKey_CurrencyNo,
	HoldKey_MatchDate,
	HoldKey_MatchTime,
	HoldKey_NoticeTime,
	HoldKey_LastTradeTime,
	HoldKey_LastQty,
	HoldKey_Default
};

enum SortLiquidateKey
{
	LiquidateKey_LiquidateNo = 0,
	LiquidateKey_UserNo,
	LiquidateKey_ExchangeNo,
	LiquidateKey_CommodityNo,
	LiquidateKey_Contract,
	LiquidateKey_CoverDirect,
	LiquidateKey_PreSettle,
	LiquidateKey_OpenPrice,

	LiquidateKey_CoverPrice,
	LiquidateKey_CoverQty,
	LiquidateKey_CloseProfit,
	LiquidateKey_CloseProfitTBT,
	LiquidateKey_CommodityType,

	LiquidateKey_Currency,
	LiquidateKey_OpenMatchNo,
	LiquidateKey_CoverMatchNo,
	LiquidateKey_UpperNo
};

enum SortFundKey
{
	FundKey_UserNo = 0
};

//字体大小
enum Font_Size
{
	Font_Small = 0, 
	Font_Middle, 
	Font_Large
};
//成交 ，持仓
#define LeftSign				30
//风险可配置参数
typedef struct tagRiskParam
{
	double	InterestRate;
	int		Steps;
	int		VolatilityRange;
}RiskParam;

struct tagHoldInfo
{
	int			ncurbuy;
	int			ncursell;
	int			nallbuy;
	int			nallsell;
	double		delta;
	double		gamma;

	tagHoldInfo()
	{
		memset(this, 0, sizeof(tagHoldInfo));
	}
};
//////////////////////////////////////////////////////////////////////////
typedef bool(*pfunc)(const void* psrc, const void* pdes);
class TContainer : public IDataContainer
{
public:
	virtual uint __cdecl size()
	{
		return m_stldata.size();
	}
	virtual const void * __cdecl at(uint i)
	{
		return m_stldata.at(i);
	}
	virtual void __cdecl clear()
	{
		m_stldata.clear();
	}
	virtual void __cdecl del(const void * data)
	{
		std::vector<const void*>::iterator itor;
		itor = std::find(m_stldata.begin(), m_stldata.end(), data);
		if (itor != m_stldata.end())
			m_stldata.erase(itor);
	}
	virtual void __cdecl push_back(const void * data)
	{
		m_stldata.push_back((const void*)data);
	}
	bool __cdecl pushbackex(const void* data)
	{
		if (std::find(m_stldata.begin(), m_stldata.end(), data) != m_stldata.end())
			return false;
		m_stldata.push_back((const void*)data);
		return true;
	}
	void		sort(bool bupper/* = true*/, pfunc func)
	{
		if (bupper)
			std::sort(m_stldata.begin(), m_stldata.end(), func);
		else
		{
			std::sort(m_stldata.begin(), m_stldata.end(), func);
			std::reverse(m_stldata.begin(), m_stldata.end());
		}
	}
	std::vector<const void*>* GetBaseData(){ return &m_stldata; }
private:
	std::vector<const void*> m_stldata;
};

typedef TContainer		TOrderContainer;
typedef TContainer		TPosContainer;
typedef TContainer		TMatchContainer;
typedef TContainer		TMoneyContainer;
typedef TContainer		TLiquidateContainer;
typedef TContainer		TUserInfoContainer;
typedef TContainer		TExchangeContainer;
typedef TContainer		TCommodityContainer;
typedef TContainer		THisOrderProcContainer;
typedef TContainer		TCurrencyContainer;
typedef TContainer		TUpperContainer;
typedef TContainer		TLoginContainer;
typedef TContainer		TExchangeStateContainer;
typedef TContainer		TCashContainer;
//////////////////////////////////////////////////////////////////////////
class TRowInfoContainer : public IDataContainer
{
public:
	virtual uint __cdecl size()
	{
		return m_stldata.size();
	}
	virtual const void * __cdecl at(uint i)
	{
		return &m_stldata.at(i);
	}
	virtual void __cdecl del(const void * data)
	{
		return;
	}
	TRowInfo GetData(uint i)
	{
		return m_stldata.at(i);
	}
	virtual void __cdecl clear()
	{
		m_stldata.clear();
	}
	virtual void __cdecl push_back(const void * data)
	{
		TRowInfo stdata = *(TRowInfo*)data;
		m_stldata.push_back(stdata);
	}
private:
	std::vector<TRowInfo> m_stldata;
};
//////////////////////////////////////////////////////////////////////////
class TLossProfit : public IDataContainer
{
public:
	virtual uint __cdecl size()
	{
		return m_stldata.size();
	}
	virtual const void * __cdecl at(uint i)
	{
		return &m_stldata.at(i);
	}
	TPositionShow GetData(uint i)
	{
		return m_stldata.at(i);
	}
	virtual void __cdecl clear()
	{
		m_stldata.clear();
	}
	virtual void __cdecl del(const void * data)
	{
		return;
	}
	virtual void __cdecl push_back(const void * data)
	{
		TPositionShow stdata = *(TPositionShow*)data;
		m_stldata.push_back(stdata);
	}
private:
	std::vector<TPositionShow> m_stldata;
};
//////////////////////////////////////////////////////////////////////////
class THisMatch : public IDataContainer
{
public:
	virtual uint __cdecl size()
	{
		return m_stldata.size();
	}
	virtual const void * __cdecl at(uint i)
	{
		if (i >= m_stldata.size())
			return NULL;
		return &m_stldata.at(i);
	}
	TMatch GetData(uint i)
	{
		return m_stldata.at(i);
	}
	TOrder* GetOrder(uint i)
	{
		if (i >= m_stlorder.size())
			return NULL;
		return &m_stlorder.at(i);
	}
	virtual void __cdecl clear()
	{
		OutputDebugStringA("__cdecl clear\r\n");
		m_stldata.clear();
		m_stlorder.clear();
	}
	virtual void __cdecl del(const void * data)
	{
		return;
	}
	virtual void __cdecl push_back(const void * data)
	{
		TMatch stdata = *(TMatch*)data;
		TOrder storder;
		if (stdata.pOrder)
			storder = *(stdata.pOrder);
		else
			memset(&storder, 0, sizeof(TOrder));
		m_stlorder.push_back(storder);
		m_stldata.push_back(stdata);
	}
private:
	std::vector<TMatch> m_stldata;
	std::vector<TOrder>	m_stlorder;
};

class THisCash : public IDataContainer
{
public:
	virtual uint __cdecl size()
	{
		return m_stldata.size();
	}
	virtual const void * __cdecl at(uint i)
	{
		if (i >= m_stldata.size())
			return NULL;
		return &m_stldata.at(i);
	}
	TCash GetData(uint i)
	{
		return m_stldata.at(i);
	}
	TUserInfo* GetUser(uint i)
	{
		if (i >= m_stluser.size())
			return NULL;
		return &m_stluser.at(i);
	}
	virtual void __cdecl clear()
	{
		OutputDebugStringA("__cdecl clear\r\n");
		m_stldata.clear();
		m_stluser.clear();
	}
	virtual void __cdecl del(const void * data)
	{
		return;
	}
	virtual void __cdecl push_back(const void * data)
	{
		TCash stdata = *(TCash*)data;
		TUserInfo stuser;
		if (stdata.pUserInfo)
			stuser = *(stdata.pUserInfo);
		else
			memset(&stuser, 0, sizeof(TUserInfo));
		m_stluser.push_back(stuser);
		m_stldata.push_back(stdata);
	}
private:
	std::vector<TCash>		m_stldata;
	std::vector<TUserInfo>	m_stluser;
};
//////////////////////////////////////////////////////////////////////////
typedef bool(*pentrust)(const TOrder psrc, const TOrder pdes);
class THisEntrust : public IDataContainer
{
public:
	virtual uint __cdecl size()
	{
		return m_stldata.size();
	}
	virtual const void * __cdecl at(uint i)
	{
		if (i >= m_stldata.size())
			return NULL;
		return &m_stldata.at(i);
	}
	TOrder GetData(uint i)
	{
		return m_stldata.at(i);
	}
	virtual void __cdecl clear()
	{
		m_stldata.clear();
	}
	virtual void __cdecl del(const void * data)
	{
		return;
	}
	virtual void __cdecl push_back(const void * data)
	{
		TOrder stdata = *(TOrder*)data;
		m_stldata.push_back(stdata);
	}

	void sort(pentrust func)
	{
		std::sort(m_stldata.rbegin(), m_stldata.rend(), func);
	}
private:
	std::vector<TOrder> m_stldata;
};

typedef struct tagHistoryMatch{
	HWND	hwnd;
	bool	benableqry;
	tagHistoryMatch()
	{
		hwnd = NULL;
		benableqry = true;
	}
}HistoryMatch;

#define DaySecond			60*60*24
#endif