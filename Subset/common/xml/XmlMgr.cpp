#include "XmlMgr.h"
#include "load_rc.h"
#include "UIDesign.h"

extern HINSTANCE g_hinst;


//��������
void CXmlMgr::LoadLgFontCfg(const string &str, LOGFONT & lgFont, int nHeight, int nWidth, int nEscapement, int nOrientation, int fnWeight, DWORD fdwItalic, DWORD fdwUnderline,
	DWORD fdwStrikeOut, DWORD fdwCharSet, DWORD fdwOutputPrecision, DWORD fdwClipPrecision, DWORD fdwQuality, DWORD fdwPitchAndFamily, LPCTSTR lpszFace)
{
	string strID;
	strID = str + ".lfHeight";
	lgFont.lfHeight = GetCfgValueInt(strID.c_str(), nHeight);//���Դ���Ĭ��ֵ��ȱʡ��Ϊ0�����涼����ȱʡ��
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
	GetCfgValueStr(strID.c_str(), tmp, LF_FACESIZE, LoadRC::unicode_to_ansi(lpszFace).c_str());//���Ը�������ֶ��ж��Ƿ�cfg��Ч
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
	//ֻ�ÿ������ݲ���
	m_vecTableData = that.m_vecTableData;
	//�������
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
		//��ǰ����
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
			//��ǰ�Ľ�
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
				//ÿһ��������
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
					//Ĭ��ֵ�Ƿ�
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
	//˵�������ļ����Ѿ���� �������ܱ�֤�������þ���map�У�û�е�ҪȡĬ��ֵ
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
	if (NULL == GetXmlFieldElement(szField))//�򲻴���
	{
		TiXmlElement tempElement("ConfigField");
		tempElement.SetAttribute("id", szField);
		TiXmlElement* pRootElem = m_pDataXmlFile->RootElement();
		TiXmlNode *pNode = pRootElem->InsertEndChild(tempElement);
		m_pDataXmlFile->SaveFile(m_strXmlFile.c_str());
		//����map�ڴ�
		TMapCfgField::iterator itor = m_mapCfgData.find(szField);
		if (itor == m_mapCfgData.end())//û���ҵ�
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
	AddXmlField(szField);				//�򲻴���

	if (NULL == GetXmlFieldElement(szField, szSection))	//�β�����
	{
		TiXmlElement tempElement("ConfigSection");
		tempElement.SetAttribute("id", szSection);
		TiXmlElement *pParentElem = GetXmlFieldElement(szField);
		pParentElem->InsertEndChild(tempElement);
		m_pDataXmlFile->SaveFile(m_strXmlFile.c_str());
		//����map�ڴ�
		TMapCfgField::iterator iterField = m_mapCfgData.find(szField);
		if (iterField == m_mapCfgData.end())//û���ҵ�
		{
			SCfgFieldData cfgFieldData;
			cfgFieldData.strDesc;
			m_mapCfgData.insert(make_pair(szField, cfgFieldData));
		}
		else//�ҵ��� ������section
		{
			TMapCfgSection::const_iterator iterSection = iterField->second.mapCfgSecs.find(szSection);
			if (iterSection == iterField->second.mapCfgSecs.end())//û�ҵ���
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
	//���浽�����ļ���,���ж���section�Ƿ���ڣ������������ӽڵ� ������map�ڴ�
	TMapCfgField::iterator iterField = m_mapCfgData.find(szFieldID);
	if (iterField == m_mapCfgData.end())//�򲻴���
	{
		AddXmlField(szFieldID);
		iterField = m_mapCfgData.find(szFieldID);
	}
	TMapCfgSection::iterator iterSection = iterField->second.mapCfgSecs.find(szSectionID);
	if (iterSection == iterField->second.mapCfgSecs.end())//����߶β�����
		AddXmlSection(szFieldID, szSectionID);
}
bool CXmlMgr::SaveToXml()
{
	TiXmlPrinter printer;
	m_pDataXmlFile->Accept(&printer);

	//����xml_str���ļ�
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
		//�ڴ˶�ȡ�����ļ�����������ļ���ȫҲû��ϵ���ڽ�����ʾʱ�����û����ӿڻ�ȡʱ���¾��У�һ��Ҫ��֤��ʱ���£������ļ���û�о�Ҫ��Ĭ�ϵ�
		ParaseConfigField(pRoot, m_mapCfgData);
	}
	else						
	{
		//�����ھʹ���һ�� ��������ȡĬ��ֵ
		string str(m_pDataXmlFile->ErrorDesc());
		if (0 == strcmp(str.c_str(), "Error document empty."))
		{
			//�ļ����ڵ���Ϊ�գ�����һ��default��xml�ļ�
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
			//�ļ������ڴ���һ��xml�ļ�
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
	//������ �Ҳ������ӽڵ� ����map
	if (NULL == GetXmlFieldElement(cfgID->szField))
		AddXmlField(cfgID->szField);
	//���Ҷ� �Ҳ������ӽڵ� ����map
	if (NULL == GetXmlFieldElement(cfgID->szField, cfgID->szSection))
		AddXmlSection(cfgID->szField, cfgID->szSection);

	//xml�в���item ���Ҳ��� ���ӽڵ� ����map
	TiXmlElement* pXmlElm = GetXmlElement(cfgID);
	if (NULL == pXmlElm)//������ ����
	{
		//����������
		pXmlElm = GetXmlFieldElement(cfgID->szField, cfgID->szSection);
		if (NULL == pXmlElm)
			return /*-100*/false;
		//����xml�е�����
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
		//����map
		TMapCfgField::iterator iterField = m_mapCfgData.find(cfgID->szField);
		if (iterField != m_mapCfgData.end())//�ҵ���
		{
			TMapCfgSection::iterator iterSection = iterField->second.mapCfgSecs.find(cfgID->szSection);
			if (iterSection != iterField->second.mapCfgSecs.end())//�ҵ���
				iterSection->second.mapCfgItems.insert(TMapCfgItem::value_type(cfgID->szItemID, newCfgItem));
		}
	}
	else //���� ����
	{
		pXmlElm->SetAttribute("value", szValue);
		//m_pDataXmlFile->SaveFile(m_strXmlFile.c_str());
		//�����ڴ��е�����
		TMapCfgField::iterator iterField = m_mapCfgData.find(cfgID->szField);
		if (iterField != m_mapCfgData.end())//�ҵ���
		{
			TMapCfgSection::iterator iterSection = iterField->second.mapCfgSecs.find(cfgID->szSection);
			if (iterSection != iterField->second.mapCfgSecs.end())//�ҵ���
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
	//������ �Ҳ������ӽڵ� ����map
	TMapCfgField::iterator iterField = m_mapCfgData.find(cfgID->szField);
	if (iterField == m_mapCfgData.end())//�򲻴���
	{
		//if(NULL==GetXmlFieldElement(cfgID->szField) )
		{
			AddXmlField(cfgID->szField);
			iterField = m_mapCfgData.find(cfgID->szField);
		}
	}
	//���Ҷ� �Ҳ������ӽڵ� ����map
	TMapCfgSection::iterator iterSection = iterField->second.mapCfgSecs.find(cfgID->szSection);
	if (iterSection == iterField->second.mapCfgSecs.end())
	{
		//if(NULL==GetXmlFieldElement(cfgID->szField,cfgID->szSection) )
		{
			AddXmlSection(cfgID->szField, cfgID->szSection);
			iterSection = iterField->second.mapCfgSecs.find(cfgID->szSection);
		}
	}
	//xml�в���item ���Ҳ��� ���ӽڵ� ����map
	TMapCfgItem::iterator iterCfgItem = iterSection->second.mapCfgItems.find(cfgID->szItemID);
	if (iterCfgItem == iterSection->second.mapCfgItems.end())//������ ����
	{
		//����������
		TiXmlElement* pXmlElm = GetXmlFieldElement(cfgID->szField, cfgID->szSection);
		if (NULL == pXmlElm)
		{
			return -100;
		}
		//����xml�е�����
		TiXmlElement NewVal("Config");
		pXmlElm = (TiXmlElement*)pXmlElm->InsertEndChild(NewVal);
		SCfgItem newCfgItem;
		newCfgItem.iCfgType = ITEM_TYPE_INT;
		newCfgItem.pData = (void *)iValue;
		//����map
		iterSection->second.mapCfgItems.insert(TMapCfgItem::value_type(cfgID->szItemID, newCfgItem));

		pXmlElm->SetAttribute("id", cfgID->szItemID);
		pXmlElm->SetAttribute("type", "INT");
		pXmlElm->SetAttribute("value", iValue);
		//m_pDataXmlFile->SaveFile(m_strXmlFile.c_str());
		return 0;
	}
	else//���� ����
	{
		TiXmlElement* pXmlElm = GetXmlElement(cfgID);
		pXmlElm->SetAttribute("value", iValue);
		//�����ڴ��е�����
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
	//���ҵ�Section�ڵ�
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
		// �����ӽڵ㣺Row   
		TiXmlElement Row("Row");
		//CfgItem.InsertEndChild(Row);
		TiXmlNode*p2 = p1->InsertEndChild(Row);
		const CCfgRow* pCurrRow = pNewTable->GetRow(i);
		for (int j = 0; j < pNewTable->GetColomnCount(); ++j)
		{
			// �����ӽڵ㣺Column
			//TiXmlElement *pColumn = new TiXmlElement(_T("Column"));
			TiXmlElement tmp("Column");
			//Row.InsertEndChild(tmp);
			TiXmlElement*p3 = (TiXmlElement*)p2->InsertEndChild(tmp);
			// ���ýڵ�Window��ֵ
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
	//����map
	TMapCfgField::iterator iterField = m_mapCfgData.find(cfgID->szField);
	if (iterField != m_mapCfgData.end())
	{
		TMapCfgSection::iterator iterSection = iterField->second.mapCfgSecs.find(cfgID->szSection);
		if (iterSection != iterField->second.mapCfgSecs.end())
		{
			TMapCfgItem::iterator itItem = iterSection->second.mapCfgItems.find(cfgID->szItemID);
			if (itItem == iterSection->second.mapCfgItems.end())//û���ҵ����о����� ��������û�ҵ��ŵ���AddCfgTable��
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
	//�ȸ���XML�ļ������ݣ��ٸ����ڴ�
	//��ȡ��ǰ�Ľڵ�
	//ɾ���ɽڵ�
	pXmlElm->Clear();
	for (int i = 0; i < pNewTable->GetRowCount(); ++i)
	{
		const CCfgRow* pCfgRow = pNewTable->GetRow(i);
		// �����ӽڵ㣺Row   
		TiXmlElement Row("Row");
		TiXmlNode* p1 = pXmlElm->InsertEndChild(Row);
		for (int j = 0; j < pCfgRow->GetColumnCount()/*pNewTable->GetColomnCount()*/; ++j)
		{
			// �����ӽڵ㣺Column
			TiXmlElement Column("Column");
			TiXmlElement* p2 = (TiXmlElement*)p1->InsertEndChild(Column);

			const CCfgRow* pCurrRow = pNewTable->GetRow(i);
			// ���ýڵ�Window��ֵ
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
	//������ �Ҳ������ӽڵ� ����map
	if (NULL == GetXmlFieldElement(cfgID->szField))
		AddXmlField(cfgID->szField);
	//���Ҷ� �Ҳ������ӽڵ� ����map
	if (NULL == GetXmlFieldElement(cfgID->szField, cfgID->szSection))
		AddXmlSection(cfgID->szField, cfgID->szSection);

	//xml�в���item ���Ҳ��� ���ӽڵ� ����map
	TiXmlElement* pXmlTable = GetXmlElement(cfgID);
	if (NULL == pXmlTable)
	{
		AddCfgTable(cfgID, (CCfgTable*)pData);
		//m_pDataXmlFile->SaveFile(m_strXmlFile.c_str());
		return 0;
	}

	SCfgItem* pItem = GetCfgItem(cfgID);//map���Ҳ��� ItemID �����λ���������ǲ����ܷ���null��
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
	//�滻����
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