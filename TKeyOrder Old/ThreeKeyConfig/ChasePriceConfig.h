class ChasePriceConfig : public TIxFrm
{
public:
	ChasePriceConfig();
	~ChasePriceConfig();
	void ShowFrm(HWND hParent,RECT rect, UINT show);
	void MoveWindow(int x, int y, int cx, int cy);
	void RegistSt(stChasePrice* pstChasePrice);
	void UpdateConfig();
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnCreate();
	void InitWidget();
	void OnPaint();
	void OnComboxSel(WPARAM wParam, LPARAM lParam);
	void OnSpinChange();
private:
	TStatic m_labChaseConfig;
	TStatic m_labChaseTrigger;
	TStatic m_labChaseTriggerL;
	TStatic m_labFirstChasePrice;
	TStatic m_labChasePrice;
	TStatic m_labchaseOver;
	TStatic m_labExplain;

	CxSpinEdit m_spinSecond;
	CxCombox   m_comFirstPrice;
	CxCombox   m_comChasePrice;
	CxCombox   m_comChaseOver;	
private:
	stChasePrice* m_pstChasePrice;
private:
	enum
	{
		id_editSecond,
		id_cbChasePrice,
		id_cbChaseOver,
	};
};