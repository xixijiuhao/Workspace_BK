#pragma once

#include "ITreeNode.h"
#include "xHeader.h"

//��Ԫ���ַ�����󳤶�/////////////////////////////////////////////////////////////////////////////////
#define MAX_LIST_FIELD_BUF				MAX_PATH	//MAX_LIST_FIELD_BUF����С��527����������Ӧ�п����


class CxListCtrl : public CListCtrl, public IGridRef
{
	DECLARE_DYNAMIC(CxListCtrl)
public:
	CxListCtrl(bool UseSystemHeader = false);
	virtual ~CxListCtrl();
protected:
	DECLARE_MESSAGE_MAP()
		
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);	
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	
	void DoDrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct, CDC * pDC);
private:
	UINT		m_nCXSMIcon;
	UINT		m_nCYSMIcon;
	UINT		m_nItemHeight;
	UINT		m_nLvirIcon;				//ͼ����
	bool		m_bShowCheckBox;
	bool		m_bShowTreeBtn;
	bool		m_bUseSystemHeader;
	CImageList	m_CheckImgList;
	CImageList	m_TreeImgList;
	TCHAR		m_lpBuffer[MAX_LIST_FIELD_BUF];
	CFont		m_OutFont;				   //��ΪGetFontֻ��

	UINT		m_Gap;
public:
	LOGFONT		m_LogFont;	
	COLORREF	m_BackgroundColor;			//�հ�������
	COLORREF	m_BackColor;				//��ǰ�б���
	COLORREF	m_SelBackColor;		
	COLORREF	m_TextColor;
	COLORREF	m_SelTextColor;
	COLORREF	m_LineColor;				//��������ɫ
	BOOL		m_bShowHorzLine;			//������ʾ������
	BOOL		m_bShowVertLine;			//��������ʾ������
	BOOL		m_bDrawSelFrame;			//��ʾѡ�п�
	BOOL		m_bShowLeftLine;			//��ʾ����ߣ�m_bShowVertLineΪTrueʱ��Ч


	int GetFirstSelIndex();
	void SetHeaderFont(CFont * hFont, BOOL bRedraw = TRUE);
	CFont * GetHeaderFont();
	void SetFont(CFont * cFont, BOOL bRedraw = TRUE);
	CFont * GetFont();
	void SetItemHeight(UINT nHeight);
	UINT GetItemHeight();
	UINT GetStateImageMask_0(bool Value);	//���TreeBtn�Ķ�Ӧ״̬ͼ��
	UINT GetStateImageMask_1(bool Value);	//���CheckBox�Ķ�Ӧ״̬ͼ��
	void ShowCheckBox(bool ShowCheck=false);//��ʾCheckBox
	bool ItemChecked(int iItem);			//����CheckBox��ѡ��״̬
	bool InCheckBox(int iItem);				//�жϵ�ǰ����Ƿ���CheckBox��
	void ShowTreeBtn(bool ShowTree=false);	//��ʾTreeBtn	
	bool InTreeBtn(int iItem);				//�жϵ�ǰ����Ƿ���TreeBtn��
	UINT GetColWidthDx(int iSubItem);		//��ø��еĿ���������ڱ�׼��Ȼ����ϵ�������	
	void AutoColumnWidth(int Column = -1, bool CurrPage = true);	//����Ӧ�п�Column>=0�����ָ���У��������ȫ����
	void SetSortIcon(int Column, int Sort);	//����������,���ڻ�����ͷͼ��
protected:	
	//���ͼ���������iOrderָ��Ҫ���ͼ���ڵ�Ԫ���������˳�򣬴��㿪ʼ��iIndent
	bool GetIconRect(CRect & rcIcon, int iOrder, int iItem, int iSubItem = 0);	
	bool InIcon(int iOrder, int iItem, int iSubItem = 0);

	void DoSetHeight();
public:	
/******************************************IGridRef***********************************************/
	virtual void set_draw(bool draw);
	virtual void redraw();
	virtual void redraw(size_t first_row, size_t last_row, size_t first_col, size_t last_col);
	virtual void redraw_row(size_t first, size_t last);
	virtual void redraw_col(size_t first, size_t last);
	virtual void set_row_count(size_t count);
	virtual size_t get_row_count() ;
	virtual size_t get_col_count();
	virtual void set_col_count(size_t count);
private:
/*************************************************************************************************/
	CxHeader m_Header;
public:
	virtual void PreSubclassWindow();
	bool SetHeaderDrawInfo(HeaderDrawInfo & Info);
	void GetHeaderDrawInfo(HeaderDrawInfo & Info);
};



//LV_ITEM.state�ֶβ�����////////////////////////////////////////////////////////////////////////////
//��ΪLV_ITEM��State�ֶδӵڶ����ֽڿ�ʼ�ͱ�ʾͼƬ������
//����ؼ�������ͼ�궼���Ի�ʵ�ֵģ�����State�ֶδӵڶ����ֽڿ�ʼ�Ϳ����Զ���ʹ���ˣ����Ƶ�iImage��ʵҲ�����Զ���ʹ��
#define IMAGE_BIT_COUNT					4	//ÿ����ͼ��������ռbit�������ֵ��һ����Itemǰ������ʾ��ͼ����Ҳ��һ��(4��bit���԰���16��ͼƬ������ͼ��������Ǹ���ͼ�ꡢ��ѡ��ͼ�ꡢ����չ����ϲ�ͼ��ȣ����ɶ��ƣ����֧��6��ͼ�꣬�ټ���iImage����һ��Itemǰ��������ʾ7��ͼ�꣩
#define FIRST_IMAGE_BIT					8	//��һ��ͼ��������state�ֶ��е�bitλ��

#define IMAGE_BIT(n)					(FIRST_IMAGE_BIT+(n)*IMAGE_BIT_COUNT)			//��n��ͼ��洢λ�õ���ʼbitλ��n��0��ʼ
#define IMAGE_MASK(n)					(((1<<IMAGE_BIT_COUNT) -1) << IMAGE_BIT(n))		//��n��ͼ��Ĵ�ȡ����

#define SET_STATEIMAGE_INDEX(i, n)		(((i) + 1) << IMAGE_BIT(n))						//���õ�n��ͼ������
#define GET_STATEIMAGE_INDEX(s, n)		((((s) & IMAGE_MASK(n)) >> IMAGE_BIT(n)) - 1)	//ȡ�õ�n��ͼ������


//0�� ��ͬ��OverlayIndex
#define LVIS_STATEIMAGEMASK_0			IMAGE_MASK(0)									//Index��ȡ����
#define INDEX_TO_STATEIMAGEMASK_0(i)	SET_STATEIMAGE_INDEX(i, 0)						//��IndexתΪ״ֵ̬
#define STATEIMAGEMASK_TO_INDEX_0(s)	GET_STATEIMAGE_INDEX(s, 0)						//��״ֵ̬תΪIndex

//1�� ��ͬ��StateIndex
#define LVIS_STATEIMAGEMASK_1			IMAGE_MASK(1)
#define INDEX_TO_STATEIMAGEMASK_1(i)	SET_STATEIMAGE_INDEX(i, 1)
#define STATEIMAGEMASK_TO_INDEX_1(s)	GET_STATEIMAGE_INDEX(s, 1)

//2�� ��չ��ͼ������
#define LVIS_STATEIMAGEMASK_2			IMAGE_MASK(2)
#define INDEX_TO_STATEIMAGEMASK_2(i)	SET_STATEIMAGE_INDEX(i, 2)
#define STATEIMAGEMASK_TO_INDEX_2(s)	GET_STATEIMAGE_INDEX(s, 2)

//3�� ��չ��ͼ������......
//4�� ��չ��ͼ������......
//5�� ��չ��ͼ������......

///////////////////////////////////////////////////////////////////////////////////////////////////