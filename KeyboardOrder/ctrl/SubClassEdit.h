#pragma once

class COFInputDlg;
class CSubClassEdit :public CxSubClassWnd
{
public:
	CSubClassEdit(unsigned int &, int editId, SOFCommodityInf&, COFInputDlg &);
	~CSubClassEdit();
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	//�۸���λ�ڵڼ��� ����
	unsigned int &m_nCursorPosRef;	
	//��Լ���ھ��
	int m_EditID;
	//inf
	SOFCommodityInf &m_CommodityInf;
	//�ϴν���
	HWND m_hLastFocusWnd;
	//����
	COFInputDlg &m_RefDlg;

	bool m_bHover;
	bool m_bFocus;
	bool m_bTracking;
	HBRUSH m_ClientBrush;
	HBRUSH m_hOrderBrush;
	HBRUSH m_HotOrderBrush1;
	HBRUSH m_HotOrderBrush2;
public:
	void InitEdit(HWND hwnd);
	//��ȡ�ϴν���
	HWND GetLastFocusWnd() { return m_hLastFocusWnd; }
private:
	//�Ƿ���Ҫ����
	bool IsNeedControl(SContractNoType & contract_no, SQuoteSnapShot & tmpQuotSnap);
	//��ȡ����ID
	void GetRawContractID(SContractNoType & id);
	//��ȡ�۸�С����ǰ��λ��
	void GetPriceShowInf(const SQuoteSnapShot&tmpQuotSnap, bool &bNeedPoint, int &iPointForward, int &iPointBackward);
	//��ȡ�۸�С����ǰ��λ��
	bool GetIntByPrice(int &iForward, int &iBackward, int iPrecision, double LimitUpPrice, double LimitDownPrice);
	//�Ƿ�����Ч���ַ�����
	bool IsValidPriceKeyDown(char key);
	//�Ƿ�����Ч�ļ�������
	bool IsValidPriceInput(HWND hwnd, char cCurInput, bool bNeedControl, bool bNeedPoint, int iPointForward, int iPointBackward);
};

