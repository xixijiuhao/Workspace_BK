#pragma once

enum HeaderDrawInfoItem
{
	hdiFixCol			= 1,
	hdiTextColor		= 1 << 1,
	hdiSelTextColor		= 1 << 2,
	hdiBackColor		= 1 << 3,
	hdiSelBackColor		= 1 << 4,

	hdiLineColor		= 1 << 5,
	hdiShowHorzLine		= 1 << 6,
	hdiShowVertLine		= 1 << 7,
	hdiShowLeftLine		= 1 << 8,
	hdiShowTopLine		= 1 << 9,
	hdiVertLineMargin	= 1 << 10,
	hdiHeight			= 1 << 11,

	hdiAll				= 0xfff
};

//�ֶ�����
enum FieldType
{
	ftString,
	ftNumeric,
	ftDate
};

//�ֶ���ʾ����
enum FieldShowType
{
	fstHide,	//����
	fstShow,	//��ʾ
	fstFrozen	//ѩ�أ�����ʾҲ�������Ƴ���
};

#pragma pack(push, 1)
//listctrl �ֶνṹ����
struct HeadField
{
	int				id;             //�ֶα�ţ���Ӧ��ColumnHeadField�������
	int				width;          //��ʾ���(������)
	char			text[32];		//�ֶεı���
	int				align;			//���뷽ʽ��LVCFMT_LEFT������룬LVCFMT_CENTER�����У�LVCFMT_RIGHT���Ҷ���
	FieldType		type;			//�ֶε�����Ҳ���������ϣ��������ַ�������������ֵ���������ڵȵ�
	FieldShowType	ShowType;		//��ʾ����
	int				iSortType;		//1����-1����0����
	bool			bSorted;		//��ǰ�����ֶ�
};
#pragma pack(pop)

struct HeaderDrawInfo
{
	UINT	 nSize;					//�ṹ���С
	UINT	 nMask;					//�������룬�����Ƕ��HeaderDrawInfoItem

	int		 nFixCol;				//������
	COLORREF clrBack;				//��񱳾�
	COLORREF clrSelBack;			//ѡ�����
	COLORREF clrText;				//���������ɫ
	COLORREF clrSelText;			//ѡ����������ɫ

	COLORREF clrLine;				//�����м�ָ��ߵ���ɫ
	BOOL	 bShowHorzLine;
	BOOL	 bShowVertLine;
	UINT	 nVertLineMargin;		//�����м�ָ��߾����±ߵľ���
	BOOL	 bShowLeftLine;			//��������ߣ�bShowVertLineΪTrueʱ��Ч
	BOOL	 bShowTopLine;			//���ƶ����ߣ�bShowHorzLineΪTrueʱ��Ч
	UINT	 nHeight;				//�߶�
public:
	HeaderDrawInfo(){ memset(this, 0, sizeof(*this)); nSize = sizeof(*this); }
};

class CxHeader : public CHeaderCtrl
{
public:
	CxHeader();
protected:
	HeaderDrawInfo m_DrawInfo;
	char	m_lpBuffer[MAX_PATH];
	CFont	m_OutFont;				//��ΪGetFontֻ��
	UINT	m_Gap;

	typedef POINT TSortPoints[3];
	int		m_SortItem;
	int		m_SortType;
	POINT	m_SortPoints[4];
	void GetSortIconPoints(CRect & rect, int topGap = 2, int iconHeight = 4);
public:
	LOGFONT m_LogFont;
	void SetFont(CFont * cFont, BOOL bRedraw = TRUE);
	CFont * GetFont();
	bool SetDrawInfo(HeaderDrawInfo & Info);
	void GetDrawInfo(HeaderDrawInfo & Info);
	int	 GetItemIndex(POINT pt);	
	void SetSortIcon(int Column, int Sort);
	void SetGap(UINT value = 4);
protected:
	DECLARE_MESSAGE_MAP();
	virtual BOOL OnChildNotify(UINT msg, WPARAM wp, LPARAM lp, LRESULT* pRes);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();	
	void DoOnPaint(CDC & dc);
};
