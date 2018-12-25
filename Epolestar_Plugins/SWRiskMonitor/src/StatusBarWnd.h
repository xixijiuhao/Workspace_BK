/**
* =====================================================================
*
*       [filename    ] StatusBarWnd.h
*       [author      ] Hao Lin
*       [Description ]
*       [Timestamp   ] 2017-10-12
*
* ===========Copyright (C) 2002-2017 Esunny. All rights reserved.===========
*/

#ifndef STATUSBARWND_H
#define STATUSBARWND_H

#include "IxFrm.h"

class StatusBarWnd : public TIxFrm
{
public:
    StatusBarWnd();
    ~StatusBarWnd();

    void Create();

protected:
    virtual	LRESULT	WndProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
    void OnPaint();
    void OnSize();
    void OnLButtonDown();

private:
    HBRUSH m_brushBg;
    COLORREF m_textColor;
    HFONT m_font;
};

#endif // STATUSBARWND_H