#ifndef _NOTEINF_WND_H
#define _NOTEINF_WND_H

class TNoteInfoWnd : public TModalWnd
{
public:
	TNoteInfoWnd(HWND hparent, const char* sztitle);
	~TNoteInfoWnd();
};
#endif