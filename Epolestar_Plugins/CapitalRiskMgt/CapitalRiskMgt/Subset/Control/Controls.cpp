#include "controls.h"

Edit::Edit(const HWND parent)
{
	Wnd::Create(0, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, "edit", parent, 0);
}