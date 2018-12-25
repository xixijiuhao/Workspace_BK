#include "PreInclude.h"


void deletevoidpointer(GV_ITEM* pitem)
{
	return;
	switch (pitem->emtype)
	{
	case CellType_Tree:
		if (pitem->exdata)
		{
			GridExpand* pexpand = (GridExpand*)pitem->exdata;
			delete pitem->exdata;
			pitem->exdata = NULL;
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
TGridData::TGridData()
{

}

TGridData::~TGridData()
{
	Clear();
}

//
void TGridData::InitGridData(const _uint& ncolcount, const _uint& nrowcount)
{
	_uint nsize = m_stdata.size();
	if (nsize >= nrowcount || ncolcount * nrowcount == 0) return;

	for (_uint i = nsize; i < nrowcount; i++)
	{
		MRowData stdata;
		for (_uint ncol = 0; ncol < ncolcount; ncol++)
		{
			GV_ITEM* pcell = new GV_ITEM;
			stdata.push_back(pcell);
		}
		AddGridRow(stdata);
	}
}

GV_ITEM* TGridData::GetGridCell(const _uint& ncolindex, const _uint& nrowindex)
{
	GV_ITEM* pitem = NULL;
	if (nrowindex < m_stdata.size())
	{
		MRowData rowdata;
		rowdata = m_stdata.at(nrowindex);
		if(ncolindex < rowdata.size())
			pitem = rowdata.at(ncolindex);
	}
	return pitem;
}

void TGridData::AddGridRow(const MRowData& stdata)
{
	m_stdata.push_back(stdata);
}

void TGridData::Clear()
{
	vector<MRowData>::iterator itor;
	for (itor = m_stdata.begin(); itor != m_stdata.end();/* itor++*/)
	{
		MRowData::iterator tcbitor;
		for (tcbitor = itor->begin(); tcbitor != itor->end(); tcbitor++)
		{
			GV_ITEM* pcell = *tcbitor;
			if(!pcell) continue;

			deletevoidpointer(pcell);
			delete pcell;
			pcell = NULL;
		}
		itor = m_stdata.erase(itor);
	}
}
