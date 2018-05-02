#pragma once
#include <string>
#include <vector>
#include <map>
#include "tinyxml.h"
#include "UIDesign.h"

using namespace std;

extern G_COLOREF g_ColorRefData;

#define WM_HOTKEYCHANGE	WM_USER+100
#define FIELD_ID		"CommonSet"
#define FIELD_IDH		"KeyBoardOrder"


enum BASE_TYPE_DEF
{
	BASE_TYPE_BOOL,
	BASE_TYPE_INT,
	BASE_TYPE_STR
};

////配置项ID定义，一个配置项的ID由三项来决定
//szField为所在域,szSection为域下所在的节，根据实际情况配置,szItemID为具体的配置项的ID，包含在某个节下
typedef struct _CfgItemID
{
	char szField[32];
	char szSection[32];
	char szItemID[32];
} CfgItemID;

//基本结构的定义
struct SCfgItem
{
	int iCfgType;
	string strID;
	string strDesc;
	void* pData;
};
typedef std::map<std::string, SCfgItem> TMapCfgItem;

struct  SCfgSecData
{
	std::string strDesc;
	TMapCfgItem mapCfgItems;
};
typedef std::map<std::string, SCfgSecData> TMapCfgSection;

struct SCfgFieldData
{
	std::string strDesc;
	TMapCfgSection mapCfgSecs;
};
typedef std::map<std::string, SCfgFieldData> TMapCfgField;

struct KeybColor
{
	COLORREF clrGridBk;
	COLORREF clrGridTxtUp;
	COLORREF clrGridTxtDown;
public:
	KeybColor()
	{
		clrGridBk =/* RGB(0, 0, 0)*/g_ColorRefData.GetColorBackground();
		clrGridTxtUp =/* RGB(255, 0, 0)*/g_ColorRefData.GetColorTextRed();
		clrGridTxtDown = /*RGB(0,160,0)*/g_ColorRefData.GetColorTextGreen();
	}
};

class CCfgRow 
{
public:
	CCfgRow();
	~CCfgRow();
	int __cdecl GetColumnCount() const;
	int __cdecl GetColumnType(int iCol) const;
	int __cdecl GetIntVal(int iCol) const;
	const char* __cdecl GetStrVal(int iCol) const;
	bool __cdecl ChangeCol(int iCol, int iVal);
	bool __cdecl ChangeCol(int iCol, const char* szVal);
	bool __cdecl AddCol(int iType, int iVal);
	bool __cdecl AddCol(int iType, const char* szVal);

	struct SItem
	{
		int iType;
		union
		{
			int iVal;
		};
		std::string strVal;
	};
	typedef std::vector<SItem> TVecRow;

private:
	TVecRow m_vecRowData;
};

class CCfgTable 
{
public:
	~CCfgTable();
	int __cdecl GetRowCount() const;
	int __cdecl GetColomnCount() const;
	int __cdecl GetIntVal(int iRow, int iCol) const;
	const char* __cdecl GetStrVal(int iRow, int iCol) const;
	bool __cdecl ChangeVal(int iRow, int iCol, int iNewVal);
	bool __cdecl ChangeVal(int iRow, int iCol, const char* szNewVal);
	bool __cdecl ChangeRow(int iRow, CCfgRow* pNewRow);
	bool __cdecl AddRow(CCfgRow* pNewRow);
	const CCfgRow* __cdecl GetRow(int iRow) const;

	//获取val的type
	int __cdecl GetValType(int iRow, int iCol) const ;

	CCfgTable& operator=(const CCfgTable& that);

	typedef std::vector<CCfgRow::TVecRow> TVecTable;
private:
	TVecTable m_vecTableData;
	//临时创建的CCfgrow  应该是为了考虑多线程的问题才创建的，每一次都new出来一个，拷贝出去，在拷贝的地方加锁，若直接把内存的数据给别人，别人不知道啥时候回调用，没调用一下就得考虑同步问题
	mutable std::map<int, CCfgRow*> m_mapCreated;
};


class CXmlMgr
{
public:
	CXmlMgr();
	~CXmlMgr();
public:
	//cfg type
	enum
	{
		ITEM_TYPE_BOOL,         //BOOL型
		ITEM_TYPE_INT,          //整形
		ITEM_TYPE_DWORD,		//DWORD类型
		ITEM_TYPE_STRING,       //字符串型
		ITEM_TYPE_TABLE,        //表格类型
	};
public:
	//初始化完成赋值 修改配置同步修改此数据 配置数据都是从读取
	TMapCfgField m_mapCfgData;
private:
	//xml数据
	TiXmlDocument *m_pDataXmlFile;
	//动态库名
	string m_strModule;
	//路径
	string m_strXmlFile;
	//临时用
	string m_strField;
	string m_strSection;
public:
	//全局变量 
	//XmlCfg m_xmlcfg;
private:
	//获取路径
	void GetXmlModulePath(const char *szFileName);
	//生成表格数据
	bool GeneTableData(const TiXmlElement* pConfig, CCfgTable* pTable);
	//读取xml到map中
	void ParaseConfigField(TiXmlElement* pElem, TMapCfgField & mapCfgField);
	//获取节点
	TiXmlElement* GetXmlFieldElement(const char* szField);
	//获取节点
	TiXmlElement* GetXmlFieldElement(const char* szField, const char* szSection);
	//xml增加域节点 并更新map内存
	bool AddXmlField(const char *szField);	
	//xml增加域、段节点，并更新map内存
	bool AddXmlSection(const char* szField, const char* szSection);
	//获取值
	SCfgItem* GetCfgItem(const CfgItemID* cfgID);
	//获取节点
	TiXmlElement* GetXmlElement(const CfgItemID* cfgID);
	//增加表数据
	bool UpdateTable(TiXmlElement* pXmlElm, CCfgTable* pOldTable, const CCfgTable* pNewTable);
	bool AddCfgTable(const CfgItemID* cfgID, const CCfgTable* pNewTable);
	//重载
	bool SetField(const char* szFiled);
	bool SetSection( const char* szSection);
	bool GetCfgValueStr(const char* cfgID, char* szValue, int iValueLen, const char *szDefaultValue = NULL);
	int GetCfgValueInt(const char* cfgID, int iDefault = 0);
	int GetCfgValue(const char* cfgID, void* pData);
private:
	//辅助
	void LoadLgFontCfg(const string &str,LOGFONT & lgFont,int nHeight,int nWidth,int nEscapement,int nOrientation,int fnWeight,DWORD fdwItalic,DWORD fdwUnderline,
		DWORD fdwStrikeOut,DWORD fdwCharSet,DWORD fdwOutputPrecision,DWORD fdwClipPrecision,DWORD fdwQuality,DWORD fdwPitchAndFamily,LPCTSTR lpszFace);
private:
	
public:
	//初始化
	void Init(const char *szModuleName);
	//保存xml
	void AddXmlFieldSection(const char *szFieldID,const char *szSectionID);
	//读写字符串的值
	bool GetCfgValueStr(const CfgItemID* cfgID, char* szValue, int iValueLen, const char *szDefaultValue = NULL);
	bool SetCfgValueStr(const CfgItemID* cfgID, const char* szValue);
	//读写整形的值
	int GetCfgValueInt(const CfgItemID* cfgID, int iDefault = 0);
	int SetCfgValueInt(const CfgItemID* cfgID,  int iValue);
	//读写表格的值
	int GetCfgValue(const CfgItemID* cfgID, void* pData);
	int SetCfgValue(const CfgItemID* cfgID, const void* pData);
	//保存到XML
	bool SaveToXml();

};

extern CXmlMgr &GetXmlMgr();
