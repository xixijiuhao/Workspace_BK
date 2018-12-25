#include "PreInclude.h"

KeyBoardConfig::KeyBoardConfig():
	m_pKeyBoardSet(NULL),
	m_pKeyBoardSort(NULL)
{
	//ÅäÖÃ³õÊ¼»¯
	if (!m_pKeyBoardSet)
		m_pKeyBoardSet = new KeyBoardSet();
	if (!m_pKeyBoardSort)
		m_pKeyBoardSort = new KeyBoardSort();
}

KeyBoardConfig::~KeyBoardConfig()
{
	if (m_pKeyBoardSet)
	{
		delete m_pKeyBoardSet;
		m_pKeyBoardSet = NULL;
	}
	if (m_pKeyBoardSort)
	{
		delete m_pKeyBoardSort;
		m_pKeyBoardSort = NULL;
	}
}
void KeyBoardConfig::Regist(IKeyOrderCfg* p)
{
	if (m_pKeyBoardSet)
		m_pKeyBoardSet->Regist(p);
	if (m_pKeyBoardSort)
		m_pKeyBoardSort->Regist(p);
}
void KeyBoardConfig::UnRegist(IKeyOrderCfg* p)
{
	if (m_pKeyBoardSet)
		m_pKeyBoardSet->UnRegist(p);
	if (m_pKeyBoardSort)
		m_pKeyBoardSort->UnRegist(p);
}

HWND KeyBoardConfig::GetKeyBoardSetHWND()
{
	if (m_pKeyBoardSet)
		return m_pKeyBoardSet->GetHwnd();
	else
		return NULL;
}
HWND KeyBoardConfig::GetKeyBoardSeqHWND()
{
	if (m_pKeyBoardSort)
		return m_pKeyBoardSort->GetHwnd();
	else
		return NULL;
}

void KeyBoardConfig::CreateKeyBoardSetWnd(const wchar_t* title)
{
	if (m_pKeyBoardSet)
		m_pKeyBoardSet->Create(title, NULL, WS_OVERLAPPED, 0);
}
void KeyBoardConfig::CreateKeyBoardSeqWnd(const wchar_t* title)
{
	if (m_pKeyBoardSort)
		m_pKeyBoardSort->Create(title, GetActiveWindow(), WS_POPUP, 0);
}