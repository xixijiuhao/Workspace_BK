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

////������ID���壬һ���������ID������������
//szFieldΪ������,szSectionΪ�������ڵĽڣ�����ʵ���������,szItemIDΪ������������ID��������ĳ������
typedef struct _CfgItemID
{
	char szField[32];
	char szSection[32];
	char szItemID[32];
} CfgItemID;

//�����ṹ�Ķ���
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

	//��ȡval��type
	int __cdecl GetValType(int iRow, int iCol) const ;

	CCfgTable& operator=(const CCfgTable& that);

	typedef std::vector<CCfgRow::TVecRow> TVecTable;
private:
	TVecTable m_vecTableData;
	//��ʱ������CCfgrow  Ӧ����Ϊ�˿��Ƕ��̵߳�����Ŵ����ģ�ÿһ�ζ�new����һ����������ȥ���ڿ����ĵط���������ֱ�Ӱ��ڴ�����ݸ����ˣ����˲�֪��ɶʱ��ص��ã�û����һ�¾͵ÿ���ͬ������
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
		ITEM_TYPE_BOOL,         //BOOL��
		ITEM_TYPE_INT,          //����
		ITEM_TYPE_DWORD,		//DWORD����
		ITEM_TYPE_STRING,       //�ַ�����
		ITEM_TYPE_TABLE,        //�������
	};
public:
	//��ʼ����ɸ�ֵ �޸�����ͬ���޸Ĵ����� �������ݶ��ǴӶ�ȡ
	TMapCfgField m_mapCfgData;
private:
	//xml����
	TiXmlDocument *m_pDataXmlFile;
	//��̬����
	string m_strModule;
	//·��
	string m_strXmlFile;
	//��ʱ��
	string m_strField;
	string m_strSection;
public:
	//ȫ�ֱ��� 
	//XmlCfg m_xmlcfg;
private:
	//��ȡ·��
	void GetXmlModulePath(const char *szFileName);
	//���ɱ������
	bool GeneTableData(const TiXmlElement* pConfig, CCfgTable* pTable);
	//��ȡxml��map��
	void ParaseConfigField(TiXmlElement* pElem, TMapCfgField & mapCfgField);
	//��ȡ�ڵ�
	TiXmlElement* GetXmlFieldElement(const char* szField);
	//��ȡ�ڵ�
	TiXmlElement* GetXmlFieldElement(const char* szField, const char* szSection);
	//xml������ڵ� ������map�ڴ�
	bool AddXmlField(const char *szField);	
	//xml�����򡢶νڵ㣬������map�ڴ�
	bool AddXmlSection(const char* szField, const char* szSection);
	//��ȡֵ
	SCfgItem* GetCfgItem(const CfgItemID* cfgID);
	//��ȡ�ڵ�
	TiXmlElement* GetXmlElement(const CfgItemID* cfgID);
	//���ӱ�����
	bool UpdateTable(TiXmlElement* pXmlElm, CCfgTable* pOldTable, const CCfgTable* pNewTable);
	bool AddCfgTable(const CfgItemID* cfgID, const CCfgTable* pNewTable);
	//����
	bool SetField(const char* szFiled);
	bool SetSection( const char* szSection);
	bool GetCfgValueStr(const char* cfgID, char* szValue, int iValueLen, const char *szDefaultValue = NULL);
	int GetCfgValueInt(const char* cfgID, int iDefault = 0);
	int GetCfgValue(const char* cfgID, void* pData);
private:
	//����
	void LoadLgFontCfg(const string &str,LOGFONT & lgFont,int nHeight,int nWidth,int nEscapement,int nOrientation,int fnWeight,DWORD fdwItalic,DWORD fdwUnderline,
		DWORD fdwStrikeOut,DWORD fdwCharSet,DWORD fdwOutputPrecision,DWORD fdwClipPrecision,DWORD fdwQuality,DWORD fdwPitchAndFamily,LPCTSTR lpszFace);
private:
	
public:
	//��ʼ��
	void Init(const char *szModuleName);
	//����xml
	void AddXmlFieldSection(const char *szFieldID,const char *szSectionID);
	//��д�ַ�����ֵ
	bool GetCfgValueStr(const CfgItemID* cfgID, char* szValue, int iValueLen, const char *szDefaultValue = NULL);
	bool SetCfgValueStr(const CfgItemID* cfgID, const char* szValue);
	//��д���ε�ֵ
	int GetCfgValueInt(const CfgItemID* cfgID, int iDefault = 0);
	int SetCfgValueInt(const CfgItemID* cfgID,  int iValue);
	//��д����ֵ
	int GetCfgValue(const CfgItemID* cfgID, void* pData);
	int SetCfgValue(const CfgItemID* cfgID, const void* pData);
	//���浽XML
	bool SaveToXml();

};

extern CXmlMgr &GetXmlMgr();
