#ifndef T_TABS_H
#define T_TABS_H


#define TABS_NORMAL    0x001
#define TABS_SELECTED  0x002
#define TABS_HOVER     0x004
typedef struct TabContrl
{
	int		iIndex;//��0��ʼ
	std::string	sztext;//����
	int		iState;//״̬
	bool	bEnable;//����
	bool	bShow;//��ʾ
	RECT    tabRect;//λ��

	TabContrl()
	{
		iState = TABS_NORMAL;
		memset(&tabRect, 0, sizeof(RECT));
		iIndex = -1;
		bShow = true;
		bEnable = true;
	}
	bool operator==(const TabContrl& node)
	{
		return(node.iIndex == iIndex);
	}
}TabNode;


class DrawDownTabs;
class TTabs :public TIxFrm
{
public:
	TTabs();
	~TTabs();

	bool Create(HWND hParent);
	void MoveWindow(const int& x, const int& y, const int& cx, const int& cy);
	int GetSelIndex(){ return m_iSelIndex; }
	const TabContrl* GetSelItem();
	void CalculateTabRect();
	void SetData(const std::vector<std::string>&tabstrVec);
protected:
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void	OnPaint();
	virtual void    DrawTab(TMemDC *pmemdc);
	virtual void    DrawButton(TMemDC *pmemdc);
	virtual void	OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void    OnDrawButtonDown();
private:
	int		m_iHeight;
	int		m_iWidth;
	int		m_iTabWidth;
	int		m_iTabHeight;
	int		m_iEdge;//���ε�б�߿��һ��
	int		m_iSelIndex;//ѡ�е�index
	int     m_iMaxDisplay;//�����ʾ����
	int     m_iBeginIndex;
	int		m_iEndIndex;
	HFONT	m_hfont;
	std::vector<TabContrl>		m_TabVec;
	//std::vector<TabContrl>		m_DrawMenuVec;//��������˵��еĽṹ�壿�о�û��Ҫ

	int		m_iButtonWidth;//���������
	bool	m_bButton;//�Ƿ���ʾ������
	bool    m_bButtonHover;//�Ƿ񾭹�������
	RECT	m_rcButton;


	COLORREF m_clrNormal;
	COLORREF m_clrSelect;
	COLORREF m_clrFrmHover;
	COLORREF m_clrFrame;
};



class DrawDownTabs
{

};

#endif