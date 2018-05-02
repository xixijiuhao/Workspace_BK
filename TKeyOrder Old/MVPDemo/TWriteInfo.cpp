#include "BaseInclude.h"
#include <ShObjIdl.h>
TWriteInfo::TWriteInfo()
{

}

TWriteInfo::~TWriteInfo()
{

}

void TWriteInfo::WriteTradeData(int which, IDataContainer *pContainer, HWND hwnd)
{
	IFileDialog* pfileSave = NULL;
	HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfileSave));
	if (SUCCEEDED(hr))
	{
		FILEOPENDIALOGOPTIONS dwOption;
		hr = pfileSave->GetOptions(&dwOption);
		if (SUCCEEDED(hr))
			pfileSave->SetOptions(dwOption& ~FOS_OVERWRITEPROMPT);
		LPCWSTR pszTile = L"����ֲ���Ϣ";
		LPCWSTR pszFiletype = L"txt";
		COMDLG_FILTERSPEC rgSpecp[] = { { L"�ı��ļ�", L"*.txt" } };
		hr = pfileSave->SetTitle(pszTile);
		if (SUCCEEDED(hr))
		{
			hr = pfileSave->SetDefaultExtension(pszFiletype);
			hr = pfileSave->SetFileTypes(1, rgSpecp);
		}

		if (SUCCEEDED(hr))
			hr = pfileSave->Show(hwnd);
		if (SUCCEEDED(hr))
		{
			IShellItem* pItem;
			hr = pfileSave->GetResult(&pItem);
			if (SUCCEEDED(hr))
			{
				PWSTR pszFilePath = NULL;
				if (SUCCEEDED(hr))
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
				if (SUCCEEDED(hr))
				{
					//д�ļ����ͷ��ڴ�
					if (which == writePositionData)
						WritePositonInfo(pszFilePath,pContainer,hwnd);
					else if (which == writeOrderData)
						WriteOrderInfo(pszFilePath,pContainer,hwnd);
					else if (which == writeMatchData)
						WriteMathchInfo(pszFilePath, pContainer, hwnd);
					CoTaskMemFree(pszFilePath);
				}
				pItem->Release();
			}
		}
		pfileSave->Release();
	}
}


void TWriteInfo::WritePositonInfo(LPCWSTR pszFilePath, IDataContainer *pContainer, HWND hwnd)
{
	char Name[250] = { 0 };
	char path[50] = { 0 };
	TWchar2Char(pszFilePath, path, sizeof(path));
	char szDirct[10] = { 0 };
	char szHedge[10] = { 0 };
	double dvalue = 0.0;
	FILE* pfile;
	errno_t  err = fopen_s(&pfile, path, "a+");
	if (err == 0)
	{
		SYSTEMTIME time;
		GetLocalTime(&time);
		char szTime[20] = { 0 };
		fprintf(pfile, "\n%s%d-%d-%d  %d:%d:%d\n", "����ʱ��:", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
		fprintf(pfile, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", "Ʒ��", "��Լ��", "����", "����", "����", "���־���", "��ʸ�ӯ", "Ӯ��     ", "��ֵ     ", "��֤��    ", "�ֲ־��� ", "���и�ӯ ", "������", "�����", "Ͷ��  ");
		for (int i = 0; i < pContainer->size(); i++)
		{
			TPosition* p = (TPosition*)pContainer->at(i);
			TDirect2Char(p->Direct, szDirct, sizeof(szDirct));
			TOffset2Char(p->Hedge, szHedge, sizeof(szHedge));
			dvalue = (p->PositionPrice) * (p->PositionQty);
			fprintf(pfile, "%s\t%s\t%s\t%d\t%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%d\t%d\t%s\n", p->pCommodity->CommodityName, p->ContractNo,
				szDirct, p->PositionQty, p->CanCoverQty, p->PositionPrice, p->FloatProfitTBT, p->FloatProfit, dvalue,
				p->Deposit, p->PositionPrice, p->FloatProfit, p->PositionQty, p->CanCoverQty, szHedge);
		}
		_TMessageBox_Domodal(hwnd, "��ʾ", L"������ɣ�", TMB_OK);
		fclose(pfile);
	}
}

void TWriteInfo::WriteOrderInfo(LPCWSTR pszFilePath, IDataContainer *pContainer, HWND hwnd)
{
	char Name[250] = { 0 };
	char path[50] = { 0 };
	TWchar2Char(pszFilePath, path, sizeof(path));
	char szDirct[10] = { 0 };
	char szHedge[10] = { 0 };
	char szOffset[10] = { 0 };
	char szState[20] = { 0 };
	FILE* pfile;
	errno_t  err = fopen_s(&pfile, path, "a+");
	if (err == 0)
	{
		SYSTEMTIME time;
		GetLocalTime(&time);
		char szTime[20] = { 0 };
		fprintf(pfile, "\n%s%d-%d-%d  %d:%d:%d\n", "����ʱ�䣺", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
		fprintf(pfile, "%s\t%s\t%s\t%s  %s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t\n", "ʱ��                  ", "Ʒ��", "��Լ��", "״̬      ", "����", "��ƽ", "ί�м�   ", "ί����", "�ɳ� ", "�ѳɽ�", "Ͷ��", "ϵͳ��                ");
		for (int i = 0; i < pContainer->size(); i++)
		{
			TOrder* p = (TOrder*)pContainer->at(i);
			TDirect2Char(p->Direct, szDirct, sizeof(szDirct));
			THedge2Char(p->Hedge, szHedge, sizeof(szHedge));
			TOffset2Char(p->Offset, szOffset, sizeof(szOffset));
			OrderState2Char(p->OrderState, p->StrategyType, szState, sizeof(szState));
			fprintf(pfile, "%s\t%s\t%s\t%s%s\t%s\t%f\t%d\t%d\t%d\t%s\t%s\n", p->InsertDateTime, p->pCommodity->CommodityName, p->ContractNo,
				szState, szDirct, szOffset, p->OrderPrice, p->OrderQty, p->OrderQty - p->MatchQty, p->MatchQty,
				szHedge, p->OrderNo);
		}
		_TMessageBox_Domodal(hwnd, "��ʾ", L"������ɣ�", TMB_OK);
		fclose(pfile);
	}
}

void TWriteInfo::OrderState2Char(TOrderState& tstate, TStrategyType& ttype, char* sztext, int nsize)
{
	TOrderState2Char(tstate, ttype, sztext, nsize);
	switch (strlen(sztext))
	{
	case 2:
		strcat_s(sztext, nsize, "        ");
	case 4:
		strcat_s(sztext, nsize, "      ");
		break;
	case 6:
		strcat_s(sztext, nsize, "    ");
		break;
	case 8:
		strcat_s(sztext, nsize, "  ");
		break;
	default:
		break;
	}
}

void TWriteInfo::WriteMathchInfo(LPCWSTR pszFilePath, IDataContainer *pContainer, HWND hwnd)
{
	char Name[250] = { 0 };
	char path[50] = { 0 };
	TWchar2Char(pszFilePath, path, sizeof(path));
	char szDirct[10] = { 0 };
	char szOffset[10] = { 0 };
	FILE* pfile;
	errno_t  err = fopen_s(&pfile, path, "a+");
	if (err == 0)
	{
		SYSTEMTIME time;
		GetLocalTime(&time);
		char szTime[20] = { 0 };
		fprintf(pfile, "\n%d-%d-%d  %d:%d:%d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
		fprintf(pfile, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", "ʱ��                  ", "Ʒ��", "��Լ��", "����", "��ƽ  ", "�ɽ���   ", "ƽ��ӯ��", "�ɽ��� ", "ϵͳ��               ", "�ɽ���");
		for (int i = 0; i < pContainer->size(); i++)
		{
			TMatch* p = (TMatch*)pContainer->at(i);
			TDirect2Char(p->Direct, szDirct, sizeof(szDirct));
			TOffset2Char(p->Offset, szOffset, sizeof(szOffset));
			fprintf(pfile, "%s\t%s\t%s\t%s\t%s\t%f\t%f\t%d\t%s\t%s\n", p->MatchDateTime, p->pCommodity->CommodityName, p->ContractNo,
				szDirct, szOffset, p->MatchPrice, p->CoverProfit, p->MatchQty, p->ExchangeMatchNo, p->MatchNo);
		}
		_TMessageBox_Domodal(hwnd, "��ʾ", L"������ɣ�", TMB_OK);
		fclose(pfile);
	}
}