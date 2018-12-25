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
	virtual void OnTimer(WPARAM wParam, LPARAM lParam);
	virtual void OnMouseMove(WPARAM wParam, LPARAM lParam);
	virtual void OnMouseLeave(WPARAM wParam, LPARAM lParam);
	virtual void OnKeyDown(WPARAM wParam, LPARAM lParam, HWND hparent);
	virtual void OnKeyUp();
	virtual void OnPinDown();
	virtual void OnPinUp();
	virtual void ResetTimer();//2016.3.1���ӳ�������

public:
	//���ú��֣���������ʾ
	void SetCharacter(const char *ptext);
	//���ø�ʽ��step�ǲ�����dots��С������ʾ��λ
	void SetFormat(double step, int dots){ m_dStep = step; m_iNumOfDot = dots; }
	void SetClrPinClk(COLORREF color){ m_clrPinClk = color; };
	void SetClrPinNormal(COLORREF color){ m_clrPinNormal = color; };
	void SetClrPinHover(COLORREF color){ m_clrPinHover = color; };
	void SetClrPinUnable(COLORREF color){ m_clrPinUnable = color; };
	void SetRange(double Min, double Max){ m_drangeMax = Max; m_drangeMin = Min; }

private:
	void ValueMinus(double Value);
	void ValueAdd(double Value);
private:
	RECT m_rcUpPin;
	RECT m_rcDownPin;

private:
	int m_iDelay;//��ʱ���ӳ�
	double m_dStep;//����
	bool m_benable;
	bool m_bHoverUpPin;
	bool m_bHoverDownPin;
	bool m_bClickupPin;
	bool m_bClickdownPin;
	bool m_bSetCharacter;//�Ƿ����ú���
	double m_dSetChValue;//���ú���ʱ���浱ǰֵ

	COLORREF m_clrPinClk;
	COLORREF m_clrPinNormal;
	COLORREF m_clrPinHover;
	COLORREF m_clrPinUnable;

	int     m_iNumOfDot;
	double		m_drangeMax;
	double		m_drangeMin;
};



#endif