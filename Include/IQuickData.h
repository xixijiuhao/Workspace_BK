#ifndef _IQUICK_DATA_H_
#define _IQUICK_DATA_H_

const wchar_t PLUG_QUICKMACRO_NAME[] = L"QuickMacro";
const wchar_t PLUG_QUICKMACRO_VERSION[] = L"1.0.0.1";

#pragma pack(push, 1)

enum SearchAction {
	Search_Null = 0,
	Search_Indicator = 1,
	Search_Line = 2,
	Search_Time = 3,
	Search_Tick = 4,
	Search_Contract = 5,
	Search_Stock = 6,
	Search_Auto = 7			//自选
};

enum QryError
{
	Code_Success = 0,
	Code_NotFund,		//没有索引对应数据
	Code_KeyCF,			//快捷键重复
	Code_KeyInvalid		//快捷键不能为空
};
//
typedef struct _EQuickData
{
	char				szkey[31];
	char				szname[101];
	char				szdescripe[101];
	char				szabridge[101];
	unsigned int		nindex;						//数据索引
}_quickdata;

class IQuickData
{
public:
	virtual		int			__cdecl		GetQryCount(SearchAction emtype) = NULL;
	virtual		void		__cdecl		GetGetQuick(SearchAction emtype, _quickdata* data, int ncount) = NULL;
	virtual		QryError	__cdecl		ModifyQuickKey(int nindex, char* sznewkey) = NULL;
};

#pragma pack(pop)
#endif
