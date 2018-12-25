#pragma once
#include "CxSubClassWnd.h"
#include <string>

using namespace std;

class CxStatic :public CxSubClassWnd
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
	wstring	m_strText;
	//״̬
	int		m_nState;	//0   1   2

	
	
public:
	void Init(HWND hwnd);
	void UpdateText(wstring strText, int nState);//���ⲿ����
	void Create(HWND hParent, UINT id, UINT ExStyle = 0);
	void SetPosition(int x, int y, int cx, int cy);
	void SetFont(HFONT hfont);
};

