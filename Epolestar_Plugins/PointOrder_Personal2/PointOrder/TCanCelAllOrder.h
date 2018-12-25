#pragma  once



typedef std::function<void(bool)> OnCencelNotice;

//*********����ȫ����Լ(ֻ֧�ֵ��˺�)*******//
class TCanCelAllOrder :public TradeSpiEx
{
public:
	void RegistNotice(OnCencelNotice pclNotice);
	//direct��������dNoneΪ˫�򶼳�.pstContractΪҪ�����ĺ�Լ�����ΪNone,����ȫ����Լ;
	void CanCelOrder(TContractSingleKey* pstContract);
 protected:
 	virtual void __cdecl OnOrder(const TOrder * pOrder);
private:
	void GetCancelOrderId(TContractSingleKey* pstContract);
	void CanCelAllParOrder();
private:
	OnCencelNotice m_pNotice;
	std::map<const uint, TOrder*> m_mapOrderId;
};