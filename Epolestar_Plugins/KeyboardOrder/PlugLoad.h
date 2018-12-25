#pragma once


extern ITradeApi * g_pTradeApi;
extern ITradeData * g_pTradeData;

class CPlugLoad
{
public:
	CPlugLoad();
	~CPlugLoad();
private:
	TPluginMgrApi *m_pPluginMgr;
	IConfigFrame *m_pConfigFrame;
	IMainFrame *m_pMainFrm;
private:
	//行情API
	IStarApi *m_pStarApi;
	//通用
	ICommonModule *m_pCommonModule;
	//策略单
	IStrategyTradeAPI *m_pStrategyTrade;
public:
	int m_idmenu;
private:
public:

public:
	void Init(TPluginMgrApi* p);
public:
	inline IConfigFrame *GetConfigFrame()	{ return m_pConfigFrame; }
	inline IMainFrame*GetMainFrm()			{ return m_pMainFrm; }
	inline IStarApi*GetStarApi()			{ return m_pStarApi; }

	inline ICommonModule*GetCommonModule()	{ return m_pCommonModule; }
	inline IStrategyTradeAPI*GetStrategyTradeAPI() { return m_pStrategyTrade; }
};

