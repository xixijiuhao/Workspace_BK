#pragma once

#include "winuser.h"

static UINT WM_LBUTTONCLK = RegisterWindowMessage("WM_LBUTTONCLK");
struct MouseClick
{
	WPARAM wParam;
	LPARAM lParam;
};