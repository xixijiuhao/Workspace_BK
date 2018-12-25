#pragma once

namespace DrawHelper
{
	//类似于MFC中的Draw3dRect
	void Draw3dRect(HDC hdc, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight);
	//类似于MFC中的Draw3dRect
	void Draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight);
}
