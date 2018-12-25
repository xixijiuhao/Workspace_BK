#pragma once

class KeyBoardConfig
{
public:
	KeyBoardConfig();
	~KeyBoardConfig();

	void Regist(IKeyOrderCfg* p);
	void UnRegist(IKeyOrderCfg* p);

	HWND GetKeyBoardSetHWND();
	void CreateKeyBoardSetWnd(const wchar_t* title);
	HWND GetKeyBoardSeqHWND();
	void CreateKeyBoardSeqWnd(const wchar_t* title);
private:
	KeyBoardSet* m_pKeyBoardSet;

	KeyBoardSort* m_pKeyBoardSort;

};

