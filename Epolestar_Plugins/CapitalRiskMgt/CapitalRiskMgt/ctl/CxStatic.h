#pragma once

class CxStatic : public CxSubClassWnd
{
public:
	CxStatic();
	~CxStatic();
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	void OnPaint();
private:
	
	bool	m_bHover;

	bool	m_bTracking;
	//�ı�
	wchar_t	m_strText[20];

public:
	void Init(HWND hwnd);
	void UpdateText(const wchar_t* strText);//���ⲿ����
};

