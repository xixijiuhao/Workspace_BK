#include "XmlMgr.h"
#include "load_rc.h"
#include "UIDesign.h"

extern HINSTANCE g_hinst;


//辅助函数
void CXmlMgr::LoadLgFontCfg(const string &str, LOGFONT & lgFont, int nHeight, int nWidth, int nEscapement, int nOrientation, int fnWeight, DWORD fdwItalic, DWORD fdwUnderline,
	DWORD fdwStrikeOut, DWORD fdwCharSet, DWORD fdwOutputPrecision, DWORD fdwClipPrecision, DWORD fdwQuality, DWORD fdwPitchAndFamily, LPCTSTR lpszFace)
{
	string strID;
	strID = str + ".lfHeight";
	lgFont.lfHeight = GetCfgValueInt(strID.c_str(), nHeight);//可以传入默认值，缺省的为0，下面都是用缺省的
	strID = str + ".lfWidth";
	lgFont.lfWidth = GetCfgValueInt(strID.c_str(), nWidth);
	strID = str + ".lfEscapement";
	lgFont.lfEscapement = GetCfgValueInt(strID.c_str(), nEscapement);
	strID = str + ".lfOrientation";
	lgFont.lfOrientation = GetCfgValueInt(strID.c_str(), nOrientation);
	strID = str + ".lfWeight";
	lgFont.lfWeight = GetCfgValueInt(strID.c_str(), fnWeight);
	strID = str + ".lfItalic";
	lgFont.lfItalic = GetCfgValueInt(strID.c_str(), fdwItalic);
	strID = str + ".lfUnderline";
	lgFont.lfUnderline = GetCfgValueInt(strID.c_str(), fdwUnderline);
	strID = str + ".lfStrikeOut";
	lgFont.lfStrikeOut = GetCfgValueInt(strID.c_str(), fdwStrikeOut);
	strID = str + ".lfCharSet";
	lgFont.lfCharSet = GetCfgValueInt(strID.c_str(), fdwCharSet);
	strID = str + ".lfOutPrecision";
	lgFont.lfOutPrecision = GetCfgValueInt(strID.c_str(), fdwOutputPrecision);
	strID = str + ".lfClipPrecision";
	lgFont.lfClipPrecision = GetCfgValueInt(strID.c_str(), fdwClipPrecision);
	strID = str + ".lfQuality";
	lgFont.lfQuality = GetCfgValueInt(strID.c_str(), fdwQuality);
	strID = str + ".lfPitchAndFamily";
	lgFont.lfPitchAndFamily = GetCfgValueInt(strID.c_str(), fdwPitchAndFamily);
	strID = str + ".lfFaceName";
	char tmp[LF_FACESIZE] = { 0 };
	GetCfgValueStr(strID.c_str(), tmp, LF_FACESIZE, LoadRC::unicode_to_ansi(lpszFace).c_str());//可以根据这个字段判断是否cfg有效
	wcscpy_s(lgFont.lfFaceName, LoadRC::ansi_to_unicode(tmp).c_str());
}

CCfgRow::CCfgRow()
{

}
CCfgRow::~CCfgRow()
{

}
int CCfgRow::GetColumnCount() const
{
	return m_vecRowData.size();
}
int CCfgRow::GetColumnType(int iCol) const
{
	return m_vecRowData[iCol].iType;
}
int CCfgRow::GetIntVal(int iCol) const
{
	return m_vecRowData[iCol].iVal;
}
const char* CCfgRow::GetStrVal(int iCol) const
{
	return m_vecRowData[iCol].strVal.c_str();
}
bool CCfgRow::ChangeCol(int iCol, int iVal)
{

	m_vecRowData[iCol].iVal = iVal;
	return true;
}
bool CCfgRow::ChangeCol(int iCol, const char* szVal)
{
	m_vecRowData[iCol].strVal = szVal;
	return true;
}
bool CCfgRow::AddCol(int iType, int iVal)
{
	SItem newItem;
	newItem.iType = iType;
	newItem.iVal = iVal;
	m_vecRowData.push_back(newItem);
	return true;
}
bool CCfgRow::AddCol(int iType, const char* szVal)
{
	SItem newItem;
	newItem.iType = iType;
	newItem.strVal = szVal;
	m_vecRowData.push_back(newItem);
	return true;
}
CCfgTable::~CCfgTable()
{
	std::map<int, CCfgRow*>::const_iterator iterEachRow = m_mapCreated.begin();
	for (; iterEachRow != m_mapCreated.end(); ++iterEachRow)
	{
		delete iterEachRow->second;
	}
}
int CCfgTable::GetRowCount() const
{
	return m_vecTableData.size();
}
int CCfgTable::GetColomnCount() const
{
	int iCol = 0;
	if (m_vecTableData.size() > 0)
	{
		iCol = m_vecTableData[0].size();
	}

	return iCol;
}
int CCfgTable::GetIntVal(int iRow, int iCol) const
{
	return m_vecTableData[iRow][iCol].iVal;
}
int CCfgTable::GetValType(int iRow, int iCol) const
{
	return m_vecTableData[iRow][iCol].iType;
}
const char* CCfgTable::GetStrVal(int iRow, int iCol) const
{
	return m_vecTableData[iRow][iCol].strVal.c_str();
}
bool CCfgTable::ChangeVal(int iRow, int iCol, int iNewVal)
{
	m_vecTableData[iRow][iCol].iVal = iNewVal;
	return true;
}
bool CCfgTable::ChangeVal(int iRow, int iCol, const char* szNewVal)
{
	m_vecTableData[iRow][iCol].strVal = szNewVal;
	return true;
}
bool CCfgTable::ChangeRow(int iRow, CCfgRow* pNewRow)
{
	if ((TVecTable::size_type)iRow >= m_vecTableData.size())
	{
		return false;
	}
	const int iColumnNum = pNewRow->GetColumnCount();
	for (int i = 0; i < iColumnNum; ++i)
	{
		switch (pNewRow->GetColumnType(i))
		{
			case BASE_TYPE_INT:
				m_vecTableData[iRow][i].iVal = pNewRow->GetIntVal(i);
				break;
			case BASE_TYPE_STR:
				m_vecTableData[iRow][i].strVal = pNewRow->GetStrVal(i);
				break;
		}
	}
	return true;
}
bool CCfgTable::AddRow(CCfgRow* pNewRow)
{
	CCfgRow::TVecRow newRow;
	for (int i = 0; i < pNewRow->GetColumnCount(); ++i)
	{
		CCfgRow::SItem newItem;
		newItem.iType = pNewRow->GetColumnType(i);
		switch (newItem.iType)
		{
			case BASE_TYPE_INT:
				newItem.iVal = pNewRow->GetIntVal(i);
				break;
			case BASE_TYPE_STR:
				newItem.strVal = pNewRow->GetStrVal(i);
				break;
		}
		newRow.push_back(newItem);
	}
	m_vecTableData.push_back(newRow);
	return true;
}
const CCfgRow* CCfgTable::GetRow(int iRow) const
{
	if ((TVecTable::size_type)iRow >= m_vecTableData.size())
	{
		return NULL;
	}
	CCfgRow* pRow = NULL;
	std::map<int, CCfgRow*>::iterator iterRow = m_mapCreated.find(iRow);
	if (iterRow != m_mapCreated.end())
	{
		pRow = iterRow->second;
	}
	else
	{
		pRow = new CCfgRow;
		m_mapCreated.insert(std::map<int, CCfgRow*>::value_type(iRow, pRow));
	}
	for (TVecTable::size_type i = 0; i < m_vecTableData[iRow].size(); ++i)
	{
		if (i >= ((TVecTable::size_type)pRow->GetColumnCount()))
		{
			switch (m_vecTableData[iRow][i].iType)
			{
				case BASE_TYPE_INT:
					pRow->AddCol(BASE_TYPE_INT, m_vecTableData[iRow][i].iVal);
					break;
				case BASE_TYPE_STR:
					pRow->AddCol(BASE_TYPE_STR, m_vecTableData[iRow][i].strVal.c_str());
					break;
			}
		}
		else
		{
			switch (m_vecTableData[iRow][i].iType)
			{
				case BASE_TYPE_INT:
					pRow->ChangeCol(i, m_vecTableData[iRow][i].iVal);
					break;
				case BASE_TYPE_STR:
					pRow->ChangeCol(i, m_vecTableData[iRow][i].strVal.c_str());
					break;
			}
		}
	}
	return pRow;
}
CCfgTable& CCfgTable::operator=(const CCfgTable& that)
{
	//只用拷贝数据部分
	m_vecTableData = that.m_vecTableData;
	//清除数据
	std::map<int, CCfgRow*>::const_iterator iterEachRow = m_mapCreated.begin();
	for (; iterEachRow != m_mapCreated.end(); ++iterEachRow)
	{
		delete iterEachRow->second;
	}

	return *this;
}


CXmlMgr::CXmlMgr() :m_pDataXmlFile(NULL)
, m_strModule()
{

}
CXmlMgr::~CXmlMgr()
{
	if (m_pDataXmlFile)
		delete m_pDataXmlFile;
}
void CXmlMgr::GetXmlModulePath(const char *szFileName)
{
	wchar_t szPath[MAX_PATH];
	::GetModuleFileName(NULL, szPath, MAX_PATH);
	string strXmlFile(LoadRC::unicode_to_ansi(szPath));
	int iPos = strXmlFile.rfind('\\');
	strXmlFile = strXmlFile.substr(0, iPos+1);
	strXmlFile += "config\\";
	strXmlFile = strXmlFile + m_strModule.c_str() + ".Default" + ".pri";
	m_strXmlFile = strXmlFile;
}
bool CXmlMgr::GeneTableData(const TiXmlElement* pConfig, CCfgTable* pTable)
{
	const TiXmlElement* pXmlTabElem = pConfig->FirstChildElement("Row");
	while (pXmlTabElem)
	{
		CCfgRow newRow;
		const TiXmlElement* pXmlRowElem = pXmlTabElem->FirstChildElement("Column");
		while (pXmlRowElem)
		{
			CCfgRow::SItem newItem;
			const char* szType = pXmlRowElem->Attribute("type");
			if (0 == strcmp("INT", szType))
				newRow.AddCol(BASE_TYPE_INT, atoi(pXmlRowElem->Attribute("value")));
			else if (0 == strcmp("STRING", szType))
				newRow.AddCol(BASE_TYPE_STR, pXmlRowElem->Attribute("value"));
			pXmlRowElem = pXmlRowElem->NextSiblingElement("Column");
		}
		pXmlTabElem = pXmlTabElem->NextSiblingElement("Row");
		pTable->AddRow(&newRow);
	}
	return true;
}
void CXmlMgr::ParaseConfigField(TiXmlElement* pElem, TMapCfgField & mapCfgField)
{
	std::string strCurElmVal;
	int iFieldPos = 0;
	while (pElem)
	{
		//当前的域
		strCurElmVal = pElem->Value();
		if ("ConfigField" != strCurElmVal)
		{
			pElem = pElem->NextSiblingElement();
			continue;
		}
		std::string strFieldID = pElem->Attribute("id");
		const TiXmlElement* pSection = pElem->FirstChildElement();

		SCfgFieldData cfgFieldData;
		cfgFieldData.strDesc = pElem->Attribute("name");
		while (pSection)
		{
			//当前的节
			strCurElmVal = pSection->Value();
			if ("ConfigSection" != strCurElmVal)
			{
				pSection = pSection->NextSiblingElement();
				continue;
			}

			std::string strCurrSectionID = pSection->Attribute("id");
			const TiXmlElement* pConfig = pSection->FirstChildElement();
			SCfgSecData cfgSecData;
			cfgSecData.strDesc = pSection->Attribute("name");
			while (pConfig)
			{
				//每一个配置项
				strCurElmVal = pConfig->Value();
				if ("Config" != strCurElmVal)
				{
					pConfig = pConfig->NextSiblingElement();
					continue;
				}
				const std::string strCfgID = pConfig->Attribute("id");
				SCfgItem newCfgItem;
				newCfgItem.strID = strCfgID;
				std::string strCfgType = pConfig->Attribute("type");
				if (0 == strcmp("INT", strCfgType.c_str()))
				{
					newCfgItem.iCfgType = ITEM_TYPE_INT;
					*(int*)&newCfgItem.pData = atoi(pConfig->Attribute("value"));
				}
				else if (0 == strcmp("DWORD", strCfgType.c_str()))
				{
					newCfgItem.iCfgType = ITEM_TYPE_DWORD;
					*(DWORD*)&newCfgItem.pData = atoi(pConfig->Attribute("value"));
				}
				else if (0 == strcmp("BOOL", strCfgType.c_str()))
				{
					newCfgItem.iCfgType = ITEM_TYPE_BOOL;
					const char* szVal = pConfig->Attribute("value");
					//默认值是否
					*(int*)&newCfgItem.pData = 0;
					if (strlen(szVal) >= 1)
					{
						if ('Y' == szVal[0])
						{
							*(int*)&newCfgItem.pData = 1;
						}
					}
				}
				else if (0 == strcmp("STRING", strCfgType.c_str()))
				{
					newCfgItem.iCfgType = ITEM_TYPE_STRING;
					const char* szVal = pConfig->Attribute("value");
					const int iLen = strlen(szVal) + 1;
					char* pCopyStr = new char[iLen];
					strncpy_s(pCopyStr, iLen, szVal, _TRUNCATE);
					newCfgItem.pData = pCopyStr;
				}
				else if (0 == strcmp("Table", strCfgType.c_str()))
				{
					newCfgItem.iCfgType = ITEM_TYPE_TABLE;
					CCfgTable* pTable = new CCfgTable;
					GeneTableData(pConfig, pTable);
					newCfgItem.pData = pTable;
				}
				const char* szDesc = pConfig->Attribute("desc");
				if (NULL != szDesc)
				{
					newCfgItem.strDesc = szDesc;
				}
				cfgSecData.mapCfgItems.insert(TMapCfgItem::value_type(strCfgID, newCfgItem));

				pConfig = pConfig->NextSiblingElement();
			}
			cfgFieldData.mapCfgSecs.insert(TMapCfgSection::value_type(strCurrSectionID, cfgSecData));
			const char* szDesc = pSection->Attribute("desc");
			if (NULL != szDesc)
			{
				cfgFieldData.strDesc = szDesc;
			}
			pSection = pSection->NextSiblingElement();
		}
		mapCfgField.insert(TMapCfgField::value_type(strFieldID, cfgFieldData));
		pElem = pElem->NextSiblingElement();
	}
	//说明配置文件读已经完成 但并不能保证所有配置均在map中，没有的要取默认值
	return;
}
TiXmlElement* CXmlMgr::GetXmlFieldElement(const char* szField)
{
	TiXmlElement* pRootElem = m_pDataXmlFile->RootElement();
	TiXmlElement* pCurrElem = pRootElem->FirstChildElement("ConfigField");
	while (pCurrElem)
	{
		if (0 == strcmp(szField, pCurrElem->Attribute("id")))
		{
			break;
		}
		pCurrElem = pCurrElem->NextSiblingElement("ConfigField");
	}
	if (NULL == pCurrElem)
	{
		return NULL;
	}
	return pCurrElem;
}
TiXmlElement* CXmlMgr::GetXmlFieldElement(const char* szField, const char* szSection)
{
	TiXmlElement* pRootElem = m_pDataXmlFile->RootElement();
	TiXmlElement* pCurrElem = pRootElem->FirstChildElement("ConfigField");
	while (pCurrElem)
	{
		if (0 == strcmp(szField, pCurrElem->Attribute("id")))
		{
			break;
		}
		pCurrElem = pCurrElem->NextSiblingElement("ConfigField");
	}
	if (NULL == pCurrElem)
	{
		return NULL;
	}
	pCurrElem = pCurrElem->FirstChildElement("ConfigSection");
	while (pCurrElem)
	{
		if (0 == strcmp(szSection, pCurrElem->Attribute("id")))
		{
			break;
		}
		pCurrElem = pCurrElem->NextSiblingElement("ConfigSection");
	}
	return pCurrElem;
}
bool CXmlMgr::AddXmlField(const char *szField)
{
	if (NULL == GetXmlFieldElement(szField))//域不存在
	{
		TiXmlElement tempElement("ConfigField");
		tempElement.SetAttribute("id", szField);
		TiXmlElement* pRootElem = m_pDataXmlFile->RootElement();
		TiXmlNode *pNode = pRootElem->InsertEndChild(tempElement);
		m_pDataXmlFile->SaveFile(m_strXmlFile.c_str());
		//更新map内存
		TMapCfgField::iterator itor = m_mapCfgData.find(szField);
		if (itor == m_mapCfgData.end())//没有找到
		{
			SCfgFieldData cfgFieldData;
			cfgFieldData.strDesc;
			m_mapCfgData.insert(make_pair(szField, cfgFieldData));
		}
	}
	return true;
}
bool CXmlMgr::AddXmlSection(const char* szField, const char* szSection)
{
	AddXmlField(szField);				//域不存在

	if (NULL == GetXmlFieldElement(szField, szSection))	//段不存在
	{
		TiXmlElement tempElement("ConfigSection");
		tempElement.SetAttribute("id", szSection);
		TiXmlElement *pParentElem = GetXmlFieldElement(szField);
		pParentElem->InsertEndChild(tempElement);
		m_pDataXmlFile->SaveFile(m_strXmlFile.c_str());
		//更新map内存
		TMapCfgField::iterator iterField = m_mapCfgData.find(szField);
		if (iterField == m_mapCfgData.end())//没有找到
		{
			SCfgFieldData cfgFieldData;
			cfgFieldData.strDesc;
			m_mapCfgData.insert(make_pair(szField, cfgFieldData));
		}
		else//找到域 继续找section
		{
			TMapCfgSection::const_iterator iterSection = iterField->second.mapCfgSecs.find(szSection);
			if (iterSection == iterField->second.mapCfgSecs.end())//没找到段
			{
				SCfgSecData tempSec;
				tempSec.strDesc;
				iterField->second.mapCfgSecs.insert(make_pair(szSection, tempSec));
			}
		}
	}
	return true;
}
void CXmlMgr::AddXmlFieldSection(const char *szFieldID,const char *szSectionID)
{
	//保存到配置文件中,先判断域、section是否存在，不存在先增加节点 并更新map内存
	TMapCfgField::iterator iterField = m_mapCfgData.find(szFieldID);
	if (iterField == m_mapCfgData.end())//域不存在
	{
		AddXmlField(szFieldID);
		iterField = m_mapCfgData.find(szFieldID);
	}
	TMapCfgSection::iterator iterSection = iterField->second.mapCfgSecs.find(szSectionID);
	if (iterSection == iterField->second.mapCfgSecs.end())//域或者段不存在
		AddXmlSection(szFieldID, szSectionID);
}
bool CXmlMgr::SaveToXml()
{
	TiXmlPrinter printer;
	m_pDataXmlFile->Accept(&printer);

	//保存xml_str到文件
	FILE* fp = 0;
	int err = fopen_s(&fp, m_strXmlFile.c_str(), "w");
	if (err || fp == 0)
		return false;

	fwrite(printer.CStr(), 1, printer.Size(), fp);
	fclose(fp);
	return true;
}
void CXmlMgr::Init(const char *szModuleName)
{
	m_strModule = szModuleName;

	if (NULL == m_pDataXmlFile)
		m_pDataXmlFile = new TiXmlDocument;

	GetXmlModulePath("");
	if (m_pDataXmlFile->LoadFile(m_strXmlFile.c_str()))
	{
		TiXmlElement* pRoot = m_pDataXmlFile->RootElement();
		pRoot = pRoot->FirstChildElement("ConfigField");
		//在此读取配置文件，如果配置文件不全也没关系，在界面显示时或者用户调接口获取时更新就行，一定要保证及时更新，配置文件中没有就要用默认的
		ParaseConfigField(pRoot, m_mapCfgData);
	}
	else						
	{
		//不存在就创建一个 所有配置取默认值
		string str(m_pDataXmlFile->ErrorDesc());
		if (0 == strcmp(str.c_str(), "Error document empty."))
		{
			//文件存在但是为空，创建一个default的xml文件
			FILE *pFile = NULL;
			fopen_s(&pFile, m_strXmlFile.c_str(), "w");
			TiXmlDeclaration tempDec("2.0", "gb2312", "");
			tempDec.Print(pFile, 0);
			TiXmlElement tempElement("DESC");
			m_pDataXmlFile->InsertEndChild(tempElement);
			m_pDataXmlFile->SaveFile(pFile);
			fclose(pFile);
			m_pDataXmlFile->LoadFile(m_strXmlFile.c_str());
		}
		else if (0 == strcmp(str.c_str(), "Failed to open file"))
		{
			//文件不存在创建一个xml文件
			FILE *pFile = NULL;
			fopen_s(&pFile, m_strXmlFile.c_str(), "a");
			if (pFile)
			{
				TiXmlDeclaration tempDec("2.0", "gb2312", "");
				tempDec.Print(pFile, 0);
				TiXmlElement tempElement("DESC");
				TiXmlNode *proot = m_pDataXmlFile->InsertEndChild(tempElement);
				m_pDataXmlFile->SaveFile(pFile);
				fclose(pFile);
				m_pDataXmlFile->LoadFile(m_strXmlFile.c_str());
			}
		}
	}
}
bool CXmlMgr::GetCfgValueStr(const CfgItemID* cfgID, char* szValue, int iValueLen, const char *szDefaultValue)
{
	SCfgItem* pItem = GetCfgItem(cfgID);
	if (NULL == pItem)
	{
		memset(szValue, 0, iValueLen);
		strncpy_s(szValue, iValueLen, szDefaultValue, _TRUNCATE);
		return false;
	}
	strncpy_s(szValue, iValueLen, (const char*)pItem->pData, _TRUNCATE);
	return true;
}
bool CXmlMgr::SetCfgValueStr(const CfgItemID* cfgID, const char* szValue)
{
	//查找域 找不到增加节点 更新map
	if (NULL == GetXmlFieldElement(cfgID->szField))
		AddXmlField(cfgID->szField);
	//查找段 找不到增加节点 更新map
	if (NULL == GetXmlFieldElement(cfgID->szField, cfgID->szSection))
		AddXmlSection(cfgID->szField, cfgID->szSection);

	//xml中查找item ，找不到 增加节点 更新map
	TiXmlElement* pXmlElm = GetXmlElement(cfgID);
	if (NULL == pXmlElm)//不存在 增加
	{
		//增加配置项
		pXmlElm = GetXmlFieldElement(cfgID->szField, cfgID->szSection);
		if (NULL == pXmlElm)
			return /*-100*/false;
		//更新xml中的数据
		TiXmlElement NewVal("Config");
		pXmlElm = (TiXmlElement*)pXmlElm->InsertEndChild(NewVal);
		SCfgItem newCfgItem;
		newCfgItem.iCfgType = ITEM_TYPE_STRING;
		const int iLen = strlen(szValue) + 1;
		char* pCopyStr = new char[iLen];
		strncpy_s(pCopyStr, iLen, szValue, _TRUNCATE);
		newCfgItem.pData = pCopyStr;
		//iterSection->second.mapCfgItems.insert(TMapCfgItem::value_type(cfgID->szItemID, newCfgItem));
		pXmlElm->SetAttribute("id", cfgID->szItemID);
		pXmlElm->SetAttribute("type", "STRING");
		pXmlElm->SetAttribute("value", szValue);
		//m_pDataXmlFile->SaveFile(m_strXmlFile.c_str());
		//更改map
		TMapCfgField::iterator iterField = m_mapCfgData.find(cfgID->szField);
		if (iterField != m_mapCfgData.end())//找到域
		{
			TMapCfgSection::iterator iterSection = iterField->second.mapCfgSecs.find(cfgID->szSection);
			if (iterSection != iterField->second.mapCfgSecs.end())//找到段
				iterSection->second.mapCfgItems.insert(TMapCfgItem::value_type(cfgID->szItemID, newCfgItem));
		}
	}
	else //存在 更新
	{
		pXmlElm->SetAttribute("value", szValue);
		//m_pDataXmlFile->SaveFile(m_strXmlFile.c_str());
		//更新内存中的数据
		TMapCfgField::iterator iterField = m_mapCfgData.find(cfgID->szField);
		if (iterField != m_mapCfgData.end())//找到域
		{
			TMapCfgSection::iterator iterSection = iterField->second.mapCfgSecs.find(cfgID->szSection);
			if (iterSection != iterField->second.mapCfgSecs.end())//找到段
			{
				TMapCfgItem::iterator iterCfgItem = iterSection->second.mapCfgItems.find(cfgID->szItemID);
				if (iterCfgItem != iterSection->second.mapCfgItems.end())
				{
					const int iLen = strlen(szValue) + 1;
					char* pCopyStr = (char*)(iterCfgItem->second.pData);
					delete[] pCopyStr;
					pCopyStr = new char[iLen];
					strncpy_s(pCopyStr, iLen, szValue, _TRUNCATE);
					iterCfgItem->second.pData = (void *)pCopyStr;
				}
			}
		}
	}
	return 0;
}
int CXmlMgr::GetCfgValueInt(const CfgItemID* cfgID, int iDefault )
{
	SCfgItem* pItem = GetCfgItem(cfgID);
	if (NULL == pItem)
		return iDefault;
	return (int)pItem->pData;
}
int CXmlMgr::SetCfgValueInt(const CfgItemID* cfgID, int iValue)
{
	//查找域 找不到增加节点 更新map
	TMapCfgField::iterator iterField = m_mapCfgData.find(cfgID->szField);
	if (iterField == m_mapCfgData.end())//域不存在
	{
		//if(NULL==GetXmlFieldElement(cfgID->szField) )
		{
			AddXmlField(cfgID->szField);
			iterField = m_mapCfgData.find(cfgID->szField);
		}
	}
	//查找段 找不到增加节点 更新map
	TMapCfgSection::iterator iterSection = iterField->second.mapCfgSecs.find(cfgID->szSection);
	if (iterSection == iterField->second.mapCfgSecs.end())
	{
		//if(NULL==GetXmlFieldElement(cfgID->szField,cfgID->szSection) )
		{
			AddXmlSection(cfgID->szField, cfgID->szSection);
			iterSection = iterField->second.mapCfgSecs.find(cfgID->szSection);
		}
	}
	//xml中查找item ，找不到 增加节点 更新map
	TMapCfgItem::iterator iterCfgItem = iterSection->second.mapCfgItems.find(cfgID->szItemID);
	if (iterCfgItem == iterSection->second.mapCfgItems.end())//不存在 增加
	{
		//增加配置项
		TiXmlElement* pXmlElm = GetXmlFieldElement(cfgID->szField, cfgID->szSection);
		if (NULL == pXmlElm)
		{
			return -100;
		}
		//更新xml中的数据
		TiXmlElement NewVal("Config");
		pXmlElm = (TiXmlElement*)pXmlElm->InsertEndChild(NewVal);
		SCfgItem newCfgItem;
		newCfgItem.iCfgType = ITEM_TYPE_INT;
		newCfgItem.pData = (void *)iValue;
		//更新map
		iterSection->second.mapCfgItems.insert(TMapCfgItem::value_type(cfgID->szItemID, newCfgItem));

		pXmlElm->SetAttribute("id", cfgID->szItemID);
		pXmlElm->SetAttribute("type", "INT");
		pXmlElm->SetAttribute("value", iValue);
		//m_pDataXmlFile->SaveFile(m_strXmlFile.c_str());
		return 0;
	}
	else//存在 更改
	{
		TiXmlElement* pXmlElm = GetXmlElement(cfgID);
		pXmlElm->SetAttribute("value", iValue);
		//更新内存中的数据
		iterCfgItem->second.pData = (void *)iValue;
		//m_pDataXmlFile->SaveFile(m_strXmlFile.c_str());
	}
	return 0;
}
int CXmlMgr::GetCfgValue(const CfgItemID* cfgID, void* pData)
{
	SCfgItem* pItem = GetCfgItem(cfgID);
	if (NULL == pItem)
	{
		return -100;
	}
	switch (pItem->iCfgType)
	{
		case ITEM_TYPE_TABLE:
			CCfgTable* pTable = (CCfgTable*)pData;
			*pTable = *(CCfgTable*)pItem->pData;
			break;
	}

	return 0;
}
bool CXmlMgr::AddCfgTable(const CfgItemID* cfgID, const CCfgTable* pNewTable)
{
	//先找到Section节点
	TiXmlElement* pCurrSection = GetXmlFieldElement(cfgID->szField, cfgID->szSection);
	if (NULL == pCurrSection)
	{
		return false;
	}
	TiXmlElement  CfgItem("Config");
	CfgItem.SetAttribute("id", cfgID->szItemID);
	CfgItem.SetAttribute("type", "Table");
	TiXmlNode*p1 = pCurrSection->InsertEndChild(CfgItem);
	for (int i = 0; i < pNewTable->GetRowCount(); ++i)
	{
		// 生成子节点：Row   
		TiXmlElement Row("Row");
		//CfgItem.InsertEndChild(Row);
		TiXmlNode*p2 = p1->InsertEndChild(Row);
		const CCfgRow* pCurrRow = pNewTable->GetRow(i);
		for (int j = 0; j < pNewTable->GetColomnCount(); ++j)
		{
			// 生成子节点：Column
			//TiXmlElement *pColumn = new TiXmlElement(_T("Column"));
			TiXmlElement tmp("Column");
			//Row.InsertEndChild(tmp);
			TiXmlElement*p3 = (TiXmlElement*)p2->InsertEndChild(tmp);
			// 设置节点Window的值
			switch (pCurrRow->GetColumnType(j))
			{
				case BASE_TYPE_INT:
					p3->SetAttribute("type", "INT");
					p3->SetAttribute("value", pCurrRow->GetIntVal(j));
					break;
				case BASE_TYPE_STR:
					p3->SetAttribute("type", "STRING");
					p3->SetAttribute("value", pCurrRow->GetStrVal(j));
					break;
				default:
					p3->SetAttribute("type", "UNKNOWN");
					p3->SetAttribute("value", "");
					break;
			}
		}
	}
	//更新map
	TMapCfgField::iterator iterField = m_mapCfgData.find(cfgID->szField);
	if (iterField != m_mapCfgData.end())
	{
		TMapCfgSection::iterator iterSection = iterField->second.mapCfgSecs.find(cfgID->szSection);
		if (iterSection != iterField->second.mapCfgSecs.end())
		{
			TMapCfgItem::iterator itItem = iterSection->second.mapCfgItems.find(cfgID->szItemID);
			if (itItem == iterSection->second.mapCfgItems.end())//没有找到，感觉多余 本来就是没找到才调用AddCfgTable的
			{
				SCfgItem tempItem;
				tempItem.iCfgType = ITEM_TYPE_TABLE;
				tempItem.strDesc = "";
				tempItem.strID = cfgID->szItemID;
				CCfgTable* pTempTable = new CCfgTable;
				*pTempTable = *(CCfgTable*)pNewTable;
				tempItem.pData = (void *)pTempTable;
				iterSection->second.mapCfgItems[cfgID->szItemID] = tempItem;
			}
		}
	}
	return true;
}
bool CXmlMgr::SetField(const char* szFiled)
{
	m_strField = szFiled;
	return true;
}
bool CXmlMgr::SetSection(const char* szSection)
{
	m_strSection = szSection;
	return true;
}
bool CXmlMgr::GetCfgValueStr(const char* cfgID, char* szValue, int iValueLen, const char *szDefaultValue )
{
	CfgItemID temp;
	strcpy_s(temp.szField, m_strField.c_str());
	strcpy_s(temp.szSection, m_strSection.c_str());
	strcpy_s(temp.szItemID, cfgID);
	return GetCfgValueStr(&temp, szValue, iValueLen, szDefaultValue);
}
int CXmlMgr::GetCfgValueInt(const char* cfgID, int iDefault )
{
	CfgItemID temp;
	strcpy_s(temp.szField, m_strField.c_str());
	strcpy_s(temp.szSection, m_strSection.c_str());
	strcpy_s(temp.szItemID, cfgID);
	return GetCfgValueInt(&temp, iDefault);
}
int CXmlMgr::GetCfgValue(const char* cfgID, void* pData)
{
	CfgItemID temp;
	strcpy_s(temp.szField, m_strField.c_str());
	strcpy_s(temp.szSection, m_strSection.c_str());
	strcpy_s(temp.szItemID, cfgID);
	return GetCfgValue(&temp, pData);
}
bool CXmlMgr::UpdateTable(TiXmlElement* pXmlElm, CCfgTable* pOldTable, const CCfgTable* pNewTable)
{
	//先更新XML文件中数据，再更新内存
	//获取当前的节点
	//删除旧节点
	pXmlElm->Clear();
	for (int i = 0; i < pNewTable->GetRowCount(); ++i)
	{
		const CCfgRow* pCfgRow = pNewTable->GetRow(i);
		// 生成子节点：Row   
		TiXmlElement Row("Row");
		TiXmlNode* p1 = pXmlElm->InsertEndChild(Row);
		for (int j = 0; j < pCfgRow->GetColumnCount()/*pNewTable->GetColomnCount()*/; ++j)
		{
			// 生成子节点：Column
			TiXmlElement Column("Column");
			TiXmlElement* p2 = (TiXmlElement*)p1->InsertEndChild(Column);

			const CCfgRow* pCurrRow = pNewTable->GetRow(i);
			// 设置节点Window的值
			switch (pCurrRow->GetColumnType(j))
			{
				case BASE_TYPE_INT:
					p2->SetAttribute("type", "INT");
					p2->SetAttribute("value", pCurrRow->GetIntVal(j));
					break;
				case BASE_TYPE_STR:
					p2->SetAttribute("type", "STRING");
					p2->SetAttribute("value", pCurrRow->GetStrVal(j));
					break;
				default:
					p2->SetAttribute("type", "UNKNOWN");
					p2->SetAttribute("value", "");
					break;
			}
		}
	}
	return true;
}
int CXmlMgr::SetCfgValue(const CfgItemID* cfgID, const void* pData)
{
	//查找域 找不到增加节点 更新map
	if (NULL == GetXmlFieldElement(cfgID->szField))
		AddXmlField(cfgID->szField);
	//查找段 找不到增加节点 更新map
	if (NULL == GetXmlFieldElement(cfgID->szField, cfgID->szSection))
		AddXmlSection(cfgID->szField, cfgID->szSection);

	//xml中查找item ，找不到 增加节点 更新map
	TiXmlElement* pXmlTable = GetXmlElement(cfgID);
	if (NULL == pXmlTable)
	{
		AddCfgTable(cfgID, (CCfgTable*)pData);
		//m_pDataXmlFile->SaveFile(m_strXmlFile.c_str());
		return 0;
	}

	SCfgItem* pItem = GetCfgItem(cfgID);//map中找不到 ItemID 在这个位置理论上是不可能返回null的
	if (NULL == pItem)
	{
		return -100;
	}

	CCfgTable* pOldTabel = (CCfgTable*)pItem->pData;
	CCfgTable* pNewTable = (CCfgTable*)pData;
	switch (pItem->iCfgType)
	{
		case ITEM_TYPE_TABLE:
			UpdateTable(pXmlTable, pOldTabel, pNewTable);
			break;
	}
	//替换数据
	delete pOldTabel;
	pOldTabel = new CCfgTable;
	*pOldTabel = *pNewTable;
	pItem->pData = pOldTabel;
	//m_pDataXmlFile->SaveFile(m_strXmlFile.c_str());

	return 0;
}
SCfgItem* CXmlMgr::GetCfgItem(const CfgItemID* cfgID)
{
	TMapCfgField::iterator iterField = m_mapCfgData.find(cfgID->szField);
	if (iterField == m_mapCfgData.end())
		return NULL;
	TMapCfgSection::iterator iterSection = iterField->second.mapCfgSecs.find(cfgID->szSection);
	if (iterSection == iterField->second.mapCfgSecs.end())
		return NULL;
	TMapCfgItem::iterator iterCfgItem = iterSection->second.mapCfgItems.find(cfgID->szItemID);
	if (iterCfgItem == iterSection->second.mapCfgItems.end())
		return NULL;
	return &(iterCfgItem->second);
}
TiXmlElement* CXmlMgr::GetXmlElement(const CfgItemID* cfgID)
{
	TiXmlElement* pRootElem = m_pDataXmlFile->RootElement();
	TiXmlElement* pCurrElem = pRootElem->FirstChildElement("ConfigField");
	while (pCurrElem)
	{
		if (0 == strcmp(cfgID->szField, pCurrElem->Attribute("id")))
			break;
		pCurrElem = pCurrElem->NextSiblingElement("ConfigField");
	}
	if (NULL == pCurrElem)
		return NULL;
	pCurrElem = pCurrElem->FirstChildElement("ConfigSection");
	while (pCurrElem)
	{
		if (0 == strcmp(cfgID->szSection, pCurrElem->Attribute("id")))
			break;
		pCurrElem = pCurrElem->NextSiblingElement("ConfigSection");
	}

	if (NULL == pCurrElem)
		return NULL;
	pCurrElem = pCurrElem->FirstChildElement("Config");
	while (pCurrElem)
	{
		if (0 == strcmp(cfgID->szItemID, pCurrElem->Attribute("id")))
			break;
		pCurrElem = pCurrElem->NextSiblingElement("Config");
	}
	return pCurrElem;
}
CXmlMgr g_XmlMgr;
CXmlMgr &GetXmlMgr()
{
	return g_XmlMgr;
}