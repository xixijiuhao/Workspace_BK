#pragma  once



typedef std::function<void(bool)> OnCencelNotice;

//*********撤除全部合约(只支持单账号)*******//
class TCanCelAllOrder :public TradeSpiEx
{
public:
	void RegistNotice(OnCencelNotice pclNotice);
	//direct撤单方向，dNone为双向都撤.pstContract为要撤单的合约，如果为None,撤出全部合约;
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