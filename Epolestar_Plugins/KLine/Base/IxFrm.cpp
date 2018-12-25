#include "IxFrm.h"

bool TIxFrm::GetInParentPoint(HWND hwnd, POINT& point)
{
	RECT r;
	POINT pp = { 0, 0 };

	GetWindowRect(hwnd, &r);

	HWND parent(::GetParent(hwnd));
	if (NULL != parent)
		ClientToScreen(parent, &pp);

	point.x = r.left - pp.x;
	point.y = r.top - pp.y;

	return true;
}

LRESULT CALLBACK TIxFrm::IxFrmProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT ret(NOT_PROCESSED);
	TIxFrm* frm(NULL);
	switch (message)
	{
	case WM_NCCREATE:
		frm = (TIxFrm*)((LPCREATESTRUCT)lParam)->lpCreateParams;
		frm->m_Hwnd = hwnd;
		SetWindowLongPtr(hwnd, 0, (LONG)frm);
		break;
	default:
		frm = (TIxFrm*)GetWindowLongPtr(hwnd, 0);
		break;
	}

	if (NULL != frm)
	{
		ret = frm->WndProc(message, wParam, lParam);
	}

	if (NOT_PROCESSED == ret)
		ret = DefWindowProc(hwnd, message, wParam, lParam);
	return ret;
}

HWND TIxFrm::GetParent()
{
	return ::GetParent(m_Hwnd);
}

bool TIxFrm::GetInParentPoint(POINT& point)
{
	return TIxFrm::GetInParentPoint(m_Hwnd, point);
}

void TIxFrm::CreateFrm(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	WNDCLASSEX wndclass;
	wndclass.cbClsExtra = 0;						//����չ���ݴ�С��һ�㲻��
	wndclass.cbWndExtra = sizeof(void*);			//������չ���ݴ�С��һ�㲻��
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.hInstance = GetModuleHandle(NULL);		//Ӧ�ó���ʵ��������û��
	wndclass.hIcon = NULL;							//Ӧ�ó����ͼ����
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW); //Ӧ�ó���Ĺ����
	wndclass.hbrBackground = NULL;					//��ֹ������˸���ÿձ�����ˢ (HBRUSH)GetStockObject(WHITE_BRUSH);//CreateSolidBrush(RGB(45, 45, 48));//
	wndclass.lpszMenuName = NULL;					//�˵�������������Դ�ķ�ʽ���ڣ�����û��
	wndclass.lpszClassName = classname;				//����������֣��������ָ���Ҳ����ظ��������ʧ�ܣ���������Ҫ�ظ��ģ�����������
	wndclass.hIconSm = NULL;
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;     //ָʾ�ര����ķ�񣬱���ָ��ˮƽ����ֱ�ػ����Ƿ���ʾ�رմ��ڵ�
	wndclass.lpfnWndProc = IxFrmProc;				//���ڹ��̺���ָ�룬���������Ҫ�Ĳ������ò���ָ����Ϣִ�еĴ��������
	RegisterClassEx(&wndclass);

	CreateWindowEx(
		exstyle, 
		classname, 
		0, 
		style,
		0, 
		0, 
		0, 
		0, 
		parent, 
		NULL, 
		GetModuleHandle(NULL), 
		this);

	HWND hwnd = GetParent();
	hwnd = NULL;
}
