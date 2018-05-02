#ifndef _DUI_TYPE_
#define _DUI_TYPE_

#ifndef _STAR_DUI_LIB_
#define _STAR_DUI_CLASS_ __declspec(dllimport)
#else
#define _STAR_DUI_CLASS_ __declspec(dllexport)
#endif

//base
enum TDuiDirection 
{
	D_LEFT,
	D_RIGHT,
	D_UP,
	D_DOWN
};

//button
enum TDuiButtonState
{
	STATE_NORMAL,
	STATE_HOVER,
	STATE_DOWN
};

//popmenu
struct TDuiPopMenuItem
{
	int										Index;						//�Զ���������
	wchar_t									Text[51];					//�������
	wchar_t									RightText[21];				//�ұ�����
	bool									Begin;						//���鿪ʼ������һ���ָ���
	bool									IsChecked;					//�Ƿ�ѡ��
	void*									Value;
};
typedef std::vector<TDuiPopMenuItem> TDuiPopMenuItemVectorType;
typedef std::multimap<int, TDuiPopMenuItem> TDuiPopMenuSubItemMapType;

//listview
typedef int				TDuiListViewIndexType;
typedef wchar_t			TDuiListViewTextType[51];
typedef unsigned char	TDuiListViewAlignType;
typedef size_t			TDuiListViewSizeType;
typedef short			TDuiListViewColWidthType;
typedef bool			TDuiListViewVisibleType;

enum TDuiListViewColType
{
	DUI_LISTVIEW_INT,
	DUI_LISTVIEW_DOUBLE,
	DUI_LISTVIEW_STR,
	DUI_LISTVIEW_WSTR,
	DUI_LISTVIEW_CHECK,						//��ѡ			����긡��Ч��
	DUI_LISTVIEW_COMBO,						//����ѡ��		����긡��Ч��
	DUI_LISTVIEW_VOID,						//ָ��ֵ
};
const wchar_t DUI_LISTVIEW_LIST_SPLIT = L'|';

struct TDuiListViewCol						
{
	TDuiListViewIndexType		Index;			//����											addcolʱ�Զ�����
	TDuiListViewColType			Type;			//����
	TDuiListViewAlignType		Align;			//���뷽ʽ
	TDuiListViewTextType		Text;			//����
	TDuiListViewColWidthType	PixelLeft;		//��߽�										addcolʱ�Զ�����
	TDuiListViewColWidthType	PixelWidth;		//�п��
	
	TDuiListViewSizeType		ValuePos;		//�п�ʼдֵλ��								addcolʱ�Զ�����		
	TDuiListViewSizeType		ValueSize;		//ֵռ�ڴ��С, һ�����ʹ��wstr����ʱʹ��		addcolʱ�Զ�����	
	TDuiListViewTextType		ComboValue;		//�б�����ʱʹ��								DUI_LISTVIEW_LIST_SPLIT�ָ�
	TDuiListViewVisibleType		Visible;		//�Ƿ���ʾ�У��������ݣ���������
};
typedef std::vector<TDuiListViewCol> TDuiListViewColVectorType;

//��ʼ������------------------------------------------------------------------------------------------------------------------------------
void _STAR_DUI_CLASS_ InitDuiRes(bool IsEnu = false);

//�ж������Ƿ����----------------------------------------------------------------------------------------------------------------------------
bool _STAR_DUI_CLASS_ IsFontExit(LPCWSTR lptext);

#endif