/**
* =====================================================================
*
*       [filename    ] MouseOrderConfigWnd.h
*       [author      ] Hao Lin
*       [Description ]
*       [Timestamp   ] 2017-07-14
*
* ===========Copyright (C) 2002-2017 Esunny. All rights reserved.===========
*/

#ifndef MOUSEORDERCONFIGWND_H
#define MOUSEORDERCONFIGWND_H

enum
{
	EN_ListStyle_Static,
	EN_ListStyle_Combox,
	EN_ParQtyStyle_Static,
	EN_ParQtyStyle_Combox,
};

typedef struct TMouseConfig
{
	int nOrderListStyle;
	int nParqtyStyle;
	TMouseConfig()
	{
		nOrderListStyle = 0;
		nParqtyStyle = 0;
	}
}TMouseConfig;

class MouseOrderConfigWnd;
class MouseConfig
{
public:
    MouseConfig();
    ~MouseConfig();

    void Subscribe(IOrderConfig *observer);
    void UnSubscribe(IOrderConfig *observer);

    HWND GetConfigWnd();
    void CreateCfgWnd(const wchar_t *title);

private:
    MouseOrderConfigWnd *m_cfgWnd;
};

class MouseOrderConfigWnd : public TIxFrm
{
public:
    MouseOrderConfigWnd();
    ~MouseOrderConfigWnd();

    void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
    void Subscribe(IOrderConfig *observer);
    void UnSubscribe(IOrderConfig *observer);

protected:
    virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
    void OnCreateWindow();
    void OnColorStyleChanged(int nID);
    void SaveConfig();

private:
    std::map<std::string, std::string> m_mapCfg;
    std::list<IOrderConfig *> m_listObservers;

    CxCombox m_ComboxStyle;

	CxCombox m_ComboxParQty;
};

#endif // MOUSEORDERCONFIGWND_H