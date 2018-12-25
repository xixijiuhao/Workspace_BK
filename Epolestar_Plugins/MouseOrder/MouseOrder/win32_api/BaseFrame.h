#ifndef _BASE_FRAME_
#define _BASE_FRAME_

//MainFrame和FloatFrame的基础窗体
//同MainFrame和FloatFrame关联的操作使用此基础类

class TBaseFrame : public TBaseFrm
{
public:
	static const int MIN_WIDTH = 314;				//窗口最小宽度
	static const int MIN_HEIGHT = 280;				//窗口最小高度
	static const int BORDER_WIDTH = 2;				//边框宽度，窗口最大化时取消边框

public:
	TBaseFrame();
	~TBaseFrame();

	void Show();

	void SetFramePos(int x, int y, int cx, int cy, UINT flag); //改变窗口大小和位置，并存入restore	

	void SetCenterShow(HWND hWnd);

	void ShowMinimize();							//最小化窗口
	
	virtual void CloseFrame();						//关闭窗口

	virtual void AdjustChildrenPos() = 0;

	void CheckAndSetMinimize();
protected:
	virtual void OnMouseMove(WPARAM wParam, LPARAM lParam);
	virtual void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	virtual void OnLButtonUp(WPARAM wParam, LPARAM lParam);
	virtual void OnLButtonDbClk(WPARAM wParam, LPARAM lParam);
	virtual void OnSetCursor(WPARAM wParam, LPARAM lParam);

private:
	int TestHit();									//仅在非最大化状态下有效
	
private:
	POINT m_MoveBeginPoint;							//窗口移动	//down  move  up
	int m_BeginHitTest;								//窗口移动	//down hittest
		
	RECT m_RestoreWindow;							//restore 位置和宽高
};













#endif