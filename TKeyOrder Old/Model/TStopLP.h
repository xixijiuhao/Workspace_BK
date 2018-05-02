#ifndef T_STOP_LOSS_PROFIT_H
#define T_STOP_LOSS_PROFIT_H

#include "TStopLPEx.h"
#include "PreInclude.h"
//*******************************************************
//ֹ��ģ�飬�����ڲ���أ�ֹ���Զ����У������ݻص�
//*********************************************************
class AbstractOrder;
class TStopLP :public TIxFrm, public TradeSpiEx, public IStopLP,public ISQuoteDataSpi
{
public:
	TStopLP();
	~TStopLP();
public:
	virtual void __stdcall OnQuote(const SContract* cont, const SQuoteUpdate* quote);
	virtual void __stdcall OnSpreadQuote(const SSpreadContract* scont) {}

	virtual void __cdecl Attach(IStopLPListener *pOb);
	//ȡ������
	virtual void __cdecl Detach(IStopLPListener *pOb);
	//��ֹ��
	virtual unsigned int __cdecl InsertLPOrder(const TStopLPInsertItem *item);
	//ɾ��ֹ��
	virtual void __cdecl DeleteLPOrder(const unsigned int KeyID);
	//�޸�ֹ��
	virtual unsigned int __cdecl ModifyLPOrder(const TStopLPitem *item);
	//����ֹ��
	//����������1(����TriggerΪtrue�������ߴ�����������
	virtual void __cdecl TriggerLPOrder(const unsigned int KeyID, bool Trigger);
	//����ֹ��
	virtual unsigned int __cdecl OpenLPOrder(const AutoStopInfo *item, const TSendOrder * stSendOrder);
	//ɾ������ֹ��
	virtual void __cdecl DeleteAllLPOrder();

	//���ָ���ͻ�����ֹ��
	//���������
	// Datas��ֹ�𵥵�������Ϣ��������Ԫ������Ϊ const TStopLPitem*
	//���������
	// UserNo���ͻ���ţ�Ϊ""��nullptrʱ�����ֿͻ�
	// Scode: ��Լ����, Ϊ""��nullptrʱ�����ֺ�Լ
	// Direct: ����ΪdBothʱ��������������dBuyΪ��dSellΪ��
	// NoTrig��δ������Ϊ����ʱ����δ������Ϊ����ʱ�����Ѵ���,Ϊ0ʱ������
	virtual void __cdecl GetALLStopLPOrder(IDataContainer &Datas, const char * UserNo = "", const char* SCode = "", TDirect direct = dBoth, int NoTrig = 0);

	//���ָ��keyID��ֹ��
	//���������
	//pItem��ֹ��ָ�룬��û�ҵ���Ӧֹ���򷵻�Ϊ��ָ��nullptr
	//���������
	//KeyID���ؼ���
	virtual void __cdecl GetSingleStopLPOrder(const TStopLPitem* pItem, const unsigned int KeyID);

protected:
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnDataChange(DataType type, TStopLPitem*pdata);

private:
	void DealQuote(LPARAM lParam);
	int  DealAttach(WPARAM wParam,LPARAM lParam);
	void DealInsertOrder(WPARAM wParam, LPARAM lParam);
	void DealDeleteOrder(WPARAM wParam, LPARAM lParam);
	void DealModifyOrder(WPARAM wParam, LPARAM lParam);
	void DealTrigOrder(WPARAM wParam, LPARAM lParam);
	void DealOnOrderCallBack(WPARAM wParam);
	void DealOnLiquidateCallBack(WPARAM wParam);

	bool DealSingleOrder(char(&contractID)[101], unsigned int i);//������ֹ��
	void SingleOrderQtyandParOrder(TStopLPitem& item, TContractKey& ContractKey);//�ֲ����Լ��ҵ����ж�
	void SendOrder(const TStopLPitem &item);//ʵʱ�������ֲֲ���ƽ�ģ���ʣ���λƽ��
	bool InsertLinkedKeyIDs(unsigned int ID1, unsigned int ID2);//������ֹ�𵥴���
	void DealLinkedKeyIDs(unsigned int keyID);//������ֹ�𵥴���
private:
	void AddSub(char(&contractID)[101], unsigned int i);
	void DelSub(char(&contractID)[101], unsigned int i);	//ɾ��ĳ������
	void DelSub();										//ɾ�����м���
private:
	//���׻ص�
	virtual void __cdecl OnLiquidate(const TLiquidate * pLiquidate);
	virtual void __cdecl OnOrder(const TOrder * pOrder);
	void GetContractKey(char* ContractKey, const TContractSingleKey* pst, int iSize);
	void GetPositionKey(char* PositionKey, TContractSingleKey* pContract, TDirect direct, int iSize);
	void GetPositionKey(char* PositionKey, const char* scode, TDirect direct, int iSize);
	bool IsStopOpen(const AutoStopInfo* stInfo);
	bool IsAutoStop(char* ContractKey);
	void InsertRemarkToStopInfo(char* cRemark,const AutoStopInfo* stStopOrder);
	bool DeleteRemarkToStopInfo(const char* cRemark);
	//�յ��ص���ֹ��
	void InsertOnOrderCallBackOrder(AutoStopInfo& stStop, const TOrder* pst);
	void SendAutoStop(AutoStopInfo& stStop, const TOrder* pst, TStopLPInsertItem& stStopOrder);
	void SendOpenStop(AutoStopInfo& stStop, const TOrder* pst, TStopLPInsertItem& stStopOrder);

	unsigned int AutoStopLoss(TStopLPInsertItem &item, const double &stopLoss, const double &basePrice = 0);
	unsigned int AutoStopProfit(TStopLPInsertItem &item, const double &stopProfit, const double &basePrice = 0);
	unsigned int AutoFloat(TStopLPInsertItem &item, const double &floatPrice, const double &basePrice = 0);
	unsigned int AutoBreakEven(TStopLPInsertItem &item, const double &BreakEvenPrice, const double &basePrice);

	void DefaultStopOrder(const TOrder* pst, const AutoStopInfo& stStop, TStopLPInsertItem& stStopOrder);
	void GetLossProfitPrice(char* ContractKey, double& dLoss, double& dProfit);


	void InsertPositionToKeyId(char* PositionKey, unsigned int KeyId);
	void GetDelKeyId(TLiquidate* pst, vector<unsigned int>& vecKeyId);

private:
	//���ݴ洢
	std::vector<AbstractOrder*>	m_Data;
	static unsigned int staticKeyID;

	//����ֹ��KeyID��map
	std::map<unsigned int, unsigned int> m_stlLinkedKeyIDs;

	//�������multimap����ԼID��ӦItem��KeyID
	std::multimap<std::string, unsigned int> m_quoteData;//ContractID->KeyID
	int m_iPositionQty;									//��ǰ����ֹ�𵥺�Լ�ĳֲ���
	int m_iOrderQty;									//��ǰ����ֹ�𵥹ҵ���
	int m_iDayPositionQty;
	int m_iDayOrderQty;
	SQuoteUpdate m_strRawQuoteData;
	SContract    m_quoteContract;
	double m_dLastPrice;								//��ǰ�����Լ�����¼�

	//ע��ص�����
	std::set<IStopLPListener*> m_setpObs;
	
	//��Ƿ������Զ�ֹ�𵥵ĵ��Ӻ�ֹ����Ϣ��map
	std::map<std::string, AutoStopInfo> m_RemarkToStopInfo;
	//�������ֹ�𵥺ͳֲ�֮�����ϵ��multimap
	std::multimap<string, unsigned int> m_PositionToKeyId;
};

//************************************************************************************
//ֹ���࣬���˲���ģʽ���⼸������Ҫ��ֹ���жϵĺ������Ժ�����������ô���������չ
//***********************************************************************************
class AbstractOrder
{
public:
	AbstractOrder(const TStopLPitem *i){ memcpy_s(&m_order, sizeof(TStopLPitem), i, sizeof(TStopLPitem)); }
	virtual bool TriggerJudge(const double &Price) = 0;
	TStopLPitem* GetOrder(){ return &m_order; }
	unsigned int GetKeyID(){ return m_order.keyID; };
protected:
	TStopLPitem m_order;
};

class TStopLossOrder :public AbstractOrder
{
public:
	TStopLossOrder(const TStopLPitem* i) :AbstractOrder(i){};
	virtual bool TriggerJudge(const double &Price);
};

class TStopProfitOrder :public AbstractOrder
{
public:
	TStopProfitOrder(const TStopLPitem* i) :AbstractOrder(i){};
	virtual bool TriggerJudge(const double &Price);
};

class TFloatStopLOrder :public AbstractOrder
{
public:
	TFloatStopLOrder(const TStopLPitem* i) :AbstractOrder(i){};
	virtual bool TriggerJudge(const double &Price);
};

class TBreakEvenOrder :public AbstractOrder
{
public:
	TBreakEvenOrder(const TStopLPitem* i) :AbstractOrder(i){};
	virtual bool TriggerJudge(const double &Price);
};
#endif