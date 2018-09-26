#ifndef _TGRID_CURSOR_H_
#define _TGRID_CURSOR_H_

class TGridCursor : public TPopupWnd
{
public:
	TGridCursor();
	~TGridCursor();
public:
	bool				Create(HWND hparent);
	void				SetBitmap(HBITMAP hbmp, POINT* pt = NULL);
public:
	SIZE				m_size;
	HBITMAP				m_hbitmap;
protected:
	virtual			void			DrawMain(TMemDC* pmemdc);
};
#endif