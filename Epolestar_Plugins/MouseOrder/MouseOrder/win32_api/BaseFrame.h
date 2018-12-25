#ifndef _BASE_FRAME_
#define _BASE_FRAME_

//MainFrame��FloatFrame�Ļ�������
//ͬMainFrame��FloatFrame�����Ĳ���ʹ�ô˻�����

class TBaseFrame : public TBaseFrm
{
public:
	static const int MIN_WIDTH = 314;				//������С���
	static const int MIN_HEIGHT = 280;				//������С�߶�
	static const int BORDER_WIDTH = 2;				//�߿��ȣ��������ʱȡ���߿�

public:
	TBaseFrame();
	~TBaseFrame();

	void Show();

	void SetFramePos(int x, int y, int cx, int cy, UINT flag); //�ı䴰�ڴ�С��λ�ã�������restore	

	void SetCenterShow(HWND hWnd);

	void ShowMinimize();							//��С������
	
	virtual void CloseFrame();						//�رմ���

	virtual void AdjustChildrenPos() = 0;

	void CheckAndSetMinimize();
protected:
	virtual void OnMouseMove(WPARAM wParam, LPARAM lParam);
	virtual void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	virtual void OnLButtonUp(WPARAM wParam, LPARAM lParam);
	virtual void OnLButtonDbClk(WPARAM wParam, LPARAM lParam);
	virtual void OnSetCursor(WPARAM wParam, LPARAM lParam);

private:
	int TestHit();									//���ڷ����״̬����Ч
	
private:
	POINT m_MoveBeginPoint;							//�����ƶ�	//down  move  up
	int m_BeginHitTest;								//�����ƶ�	//down hittest
		
	RECT m_RestoreWindow;							//restore λ�úͿ��
};













#endif