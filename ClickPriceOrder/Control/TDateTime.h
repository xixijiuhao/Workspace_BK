#ifndef _T_DATE_TIME_H_
#define _T_DATE_TIME_H_

class TDateTime
{
public:
	void Create(HWND hParent, UINT style = 0U);
	void SetTimeFormat(TCHAR* pszFormat);
	void SetTimeCurrent();
	void GetDate(char* pTime);//获取日期
	void GetDateTime(char* pTime);//获取日期时间
	void GetTime(char *pTime);
	void SetDate(char* pDate); //获取的是日期yyyy-MM-dd
	void SetTime(char* pTime);//时间格式 HH:mm:ss
	void UpdateTime();
	void Enable(bool bEnable);
	HWND GetHwnd(){ return m_hDateTime; }
	void MoveWindow(int x, int y, int cx, int cy);
	static LRESULT CALLBACK	DTProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
private:
	HWND   m_hDateTime;
};

#endif