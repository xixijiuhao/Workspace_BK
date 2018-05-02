#ifndef  __MODEL__H__
#define  __MODEL__H__

class TGlobalModel
{
public:
	TGlobalModel();
	~TGlobalModel();
public:
	double GetSuperPrice(const TContractSingleKey *pKey, TDirect direct);
	void WriteConfig();
	void ReadConfig();
	int GetSuperPriceOffset(const TContractSingleKey *pKey, TDirect direct);

	void ForeignCoverOrder(TContractSingleKey & stKey, vector<uint>* pOrderID);
	int GetForeignCloseNum(TContractSingleKey &stKey,TDirect direct);
private:
    //窗口位置数据
    void ReadPosConfile();
    void WritePosConfile();
    //选项配置
    void ReadOptionConfigFile();
    void WriteOptionConfigFile();
    //默认手数
    void ReadDefaultConfigFile();
    void WriteDefaultConfigFile();
    void InitDefaultMapFromPri(char* buffer);
    void GetContractKey(char* cContractKey, TContractKey stContract);
    void GetDefaultPriKey(char* cContractKey, stDefaultQty stQty);
    int m_DefaultQtySize;//初始化Map的个数
    //超价参数
    void ReadSuperConfigFile();
    void WriteSuperConfigFile();
    void InitSuperPriceMapFromPri(char* buffer);
    void GetSuperPrcePriKey(char* cContractKey, stSuperPrice st);
    int m_SuperPriceSize;
    //追价设置
    void ReadChaseConfigFile();
    void WriteChaseConfigFile();
    void InitChasePriceStFromPri(char* buffer);
    //止损参数
    void ReadStopLossConfigFile();
    void WriteStopLossConfigFile();
    void InitStopLossMapFromPri(char* buffer);
    void GetStopLossPriKey(char* cContractKey, stSLContractPriceMargin st);
    int m_StopLossSize;
public:
	string         m_strUserNo;         // 当前用户
	string         m_strSign;           // 当前用户服务器标识
	TUserIdentity  m_cUserType;			// 用户类型
	TKUserType	   m_utUserType;
    RECT		   m_PosRect;
    stChasePrice                         m_stChaseConfig;
	std::map<std::string, stDefaultQty>  m_mapDefaultQty;
	OptionConfig                         m_stOptionConfig;
	stStopLossConfig	                 m_stStopLossConfig;
	std::map<std::string, stSuperPrice>  m_mapSuperPrice;
	std::multimap<std::string, uint>	 m_mapForeignParOrder; //PositionNo, OrderID
};
#endif