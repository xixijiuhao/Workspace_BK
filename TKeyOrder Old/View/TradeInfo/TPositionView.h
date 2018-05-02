#pragma once

class TPositionView :public TIxFrm
{
public:
	TPositionView();
	~TPositionView();
	void Create(HWND hParent);
	void MoveWindow(int x, int y, int cx, int cy);
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnCreate();
	void OnPaint();
	void OnSize(WPARAM wParam, LPARAM lParam);
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnNotify(WPARAM wParam, LPARAM lParam);
private:
	TListView  m_posList;
	HMENU      m_hMenu;

	enum {
		IDM_RIVALCOLSE = 0, IDM_BACKHAND, IDM_CLOSE, IDM_STOPLOSS, IDM_STOPWIN, IDM_BREAKEVEN,
		IDM_TIMINGCLOSE, IDM_OVERCLOSE, IDM_MARKETCLOSE, IDM_SPECIFIEDCLOSE, IDM_CLOSESH, IDM_LOCKPOSITION,
		IDM_MOVEPOSITION, IDM_OVERBACKHAND, IDM_MARKETBACKHAND, IDM_SAVE, IDM_REFRESH, IDM_CANCELCLOSE, //³Ö²ÖÓÒ¼ü²Ëµ¥
	};
};