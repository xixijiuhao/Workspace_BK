#pragma once

namespace DrawHelper
{
	//������MFC�е�Draw3dRect
	void Draw3dRect(HDC hdc, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight);
	//������MFC�е�Draw3dRect
	void Draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight);
}
