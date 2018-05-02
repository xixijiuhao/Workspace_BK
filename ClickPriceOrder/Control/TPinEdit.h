#ifndef PINEDIT_H
#define PINEDIT_H

class TPinEdit :public TEdit
{
public:
	TPinEdit();
	~TPinEdit();

protected:
	virtual	LRESULT	WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual	void DrawMain(TMemDC* pmemdc, const RECT& rect, HBRUSH framebrush);
protected:
	virtual void OnSize();
	virtual void OnLbuttonDown(WPARAM wParam, LPARAM lParam);
	virtual void OnLbuttonup(WPARAM wParam, LPARAM lParam);
	virtual void OnMouseMove(WPARAM wParam, LPARAM lParam);
	virtual void OnMouseLeave(WPARAM wParam, LPARAM lParam);
	virtual void OnKeyDown(WPARAM wParam, LPARAM lParam, HWND hparent);
	virtual void OnPinDown();
	virtual void OnPinUp();
	virtual void OnCommand(WPARAM wParam, LPARAM lParam);
public:
	//设置汉字，仅用于显示
	void SetCharacter(const char *ptext);
	//设置格式，step是步进，dots是小数点显示几位
	void SetFormat(double step, int dots){ m_dStep = step; m_iNumOfDot = dots; }
	void SetClrPinClk(COLORREF color){ m_clrPinClk = color; };
	void SetClrPinNormal(COLORREF color){ m_clrPinNormal = color; };
	void SetClrPinHover(COLORREF color){ m_clrPinHover = color; };
	void SetClrPinUnable(COLORREF color){ m_clrPinUnable = color; };
	void SetRange(double Min, double Max){ m_drangeMax = Max; m_drangeMin = Min; }
	double GetCharacterVal(){ return m_dSetChValue; };
	bool	IsCharacter(){ return m_bSetCharacter; };
	void RemoveCharacter();
private:
	void ValueMinus(double Value);
	void ValueAdd(double Value);
private:
	RECT m_rcUpPin;
	RECT m_rcDownPin;

private:
	double m_dStep;//步进
	bool m_benable;
	bool m_bHoverUpPin;
	bool m_bHoverDownPin;
	bool m_bClickupPin;
	bool m_bClickdownPin;
	bool m_bSetCharacter;//是否设置汉字
	double m_dSetChValue;//设置汉字时保存当前值

	COLORREF m_clrPinClk;
	COLORREF m_clrPinNormal;
	COLORREF m_clrPinHover;
	COLORREF m_clrPinUnable;

	int     m_iNumOfDot;
	double		m_drangeMax;
	double		m_drangeMin;
};



#endif