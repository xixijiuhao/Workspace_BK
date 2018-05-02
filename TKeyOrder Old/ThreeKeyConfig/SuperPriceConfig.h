#ifndef SUPERPRICECONFIG_H
#define SUPERPRICECONFIG_H
class SuperPriceConfig : public TIxFrm, public IOperator
{
public:
	SuperPriceConfig();
	~SuperPriceConfig();
	void ShowFrm(HWND hParent, RECT rect, UINT show);
	void MoveWindow(int x, int y, int cx, int cy);
	void RegistSt(std::map<std::string, stSuperPrice >* pmapSuperPrice);
	void UpdateConfig();
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnCreate();
	void OnPaint();
	void OnSize(WPARAM wParam, LPARAM lParam);
	LRESULT OnNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(WPARAM wParam, LPARAM lParam);
	void OnBtnClick(WPARAM wParam, LPARAM lParam);
	void OnMouseWheel(WPARAM wParam, LPARAM lParam);
	void OnSearchCode();
	void OnBtnAdd();
	void OnBtnDelete();
	void ShowList();
	void OnMeasureListView(LPARAM lParam);
	void OnDrawListView(WPARAM wParam, LPARAM lParam);
public:
	virtual void SetContractNo(string sCode);
	virtual void SetPriceType(wstring str){};
	bool GetContract(TContractKey& ContractKey);
private:
	TStatic    m_labContractNo;
	CxEdit     m_editContractNo;
			   
	TStatic    m_labMinChangePrice;
	CxEdit     m_editMinChangePrice;
			   
	TStatic    m_labLongSuperPrice;
	CxSpinEdit m_spinLongSuperPrice;

	TStatic    m_labShortSuperPrice;
	CxSpinEdit m_spinShortSuperPrice;

	TStaticButton   m_btnAdd;
	TStaticButton   m_btnDelete;

	TStaticButton   m_btnSearch;
	CPopWinCode     m_PopupContractCode;
	CPopWinCodeList m_PopupCodeList;

	TListView       m_listSuperPrice;
private:
	TDataContainer m_clUserContainer;
	char            m_UserNo[21];
	char            m_Sign1[21];
	char            m_Sign2[21];
	std::string     m_sCode;
	bool            m_bshowCodeList;
	int             m_PriSize;
private:
	std::map<std::string, stSuperPrice>* m_mapSuperPrice;
	void InsertMap();
	void DeleteMap();
	bool FindItem(int index, stSuperPrice& st);
	void SetContent(stSuperPrice st);
	void GetContractKey(char* cContractKey, TContractSingleKey stContract);
	void GetPriKey(char* cContractKey, stSuperPrice stQty);
	bool GetMinChangePrice(TContractKey& key);
	void UpdateMapFromPriBuffer(char *buffer);
private:
	enum 
	{
		ID_EDITCONTRACTNO = 0,
		ID_BTNSEARCH,
		ID_BTNADD,
		ID_BTNDELETE,
		ID_LISTDEFAULTQTY
	};
};

#endif