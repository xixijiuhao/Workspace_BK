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
    //����λ������
    void ReadPosConfile();
    void WritePosConfile();
    //ѡ������
    void ReadOptionConfigFile();
    void WriteOptionConfigFile();
    //Ĭ������
    void ReadDefaultConfigFile();
    void WriteDefaultConfigFile();
    void InitDefaultMapFromPri(char* buffer);
    void GetContractKey(char* cContractKey, TContractKey stContract);
    void GetDefaultPriKey(char* cContractKey, stDefaultQty stQty);
    int m_DefaultQtySize;//��ʼ��Map�ĸ���
    //���۲���
    void ReadSuperConfigFile();
    void WriteSuperConfigFile();
    void InitSuperPriceMapFromPri(char* buffer);
    void GetSuperPrcePriKey(char* cContractKey, stSuperPrice st);
    int m_SuperPriceSize;
    //׷������
    void ReadChaseConfigFile();
    void WriteChaseConfigFile();
    void InitChasePriceStFromPri(char* buffer);
    //ֹ�����
    void ReadStopLossConfigFile();
    void WriteStopLossConfigFile();
    void InitStopLossMapFromPri(char* buffer);
    void GetStopLossPriKey(char* cContractKey, stSLContractPriceMargin st);
    int m_StopLossSize;
public:
	string         m_strUserNo;         // ��ǰ�û�
	string         m_strSign;           // ��ǰ�û���������ʶ
	TUserIdentity  m_cUserType;			// �û�����
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