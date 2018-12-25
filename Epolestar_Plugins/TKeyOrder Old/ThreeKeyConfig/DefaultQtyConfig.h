#ifndef DEFAULTQTYCONFIG_H
#define DEFAULTQTYCONFIG_H
class DefaultQtyConfig : public TIxFrm, public IOperator
{
public:
	DefaultQtyConfig();
	~DefaultQtyConfig();
	void ShowFrm(HWND hParent, RECT rect, UINT show);
	void MoveWindow(int x, int y, int cx, int cy);
	void RegistSt(std::map<std::string, stDefaultQty>* pmapDefaultQty);
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
	virtual void  SetContractNo(string sCode);
	virtual void SetPriceType(wstring str){};
	bool GetContract(TContractKey& ContractKey);
private:
	TStatic         m_labelContractNo;
	TStatic         m_labelDefaultQty;
	TStaticButton   m_btnAdd;
	TStaticButton   m_btnDelete;

	TListView       m_listDefaultQty;

	CxEdit          m_editContractNo;
	CxSpinEdit      m_spinDefaultQty;
	TStaticButton   m_btnSearch;
	CPopWinCode     m_PopupContractCode;
	CPopWinCodeList m_PopupCodeList;
private:
	TDataContainer  m_clUserContainer;
	char            m_UserNo[21];
	char            m_Sign1[21];
	char            m_Sign2[21];
	std::string     m_sCode;
	bool            m_bshowCodeList;
	int             m_PriSize;
private:
	std::map<std::string, stDefaultQty>* m_mapDefaultQty;
	void InsertMap();
	void DeleteMap();
	bool FindItem(int index, stDefaultQty& st);
	void SetContent(stDefaultQty st);
	void GetContractKey(char* cContractKey, TContractSingleKey stContract);
	void GetPriKey(char* cContractKey, stDefaultQty stQty);
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