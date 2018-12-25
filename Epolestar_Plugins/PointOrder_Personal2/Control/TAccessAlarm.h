#ifndef  _T_ACCESS_ALARM_H_
#define  _T_ACCESS_ALARM_H_

class TAccessAlarm :public TModalWnd
{
public:
	TAccessAlarm(HWND hwnd, char* szTitle, int type);
	~TAccessAlarm();
	bool SetDate(unsigned int serverDate, unsigned int expireDate);
protected:
	void CreateSubCtrl();
	void SetModalRect();
	void DrawMain(TMemDC* pmemdc);
	LRESULT	WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnCreate();
private:
	TStaticButton m_btnOk;
	int			  m_iFrmType;
	int           m_iDaysLeft;
	unsigned int  m_iExpireDate;

public:
	enum{
		noAuthent=1,  //未认证
		userTimeOut,  //已经到期
		daysLeftWarn,     //提示还有多少天到期
	};
};


#endif