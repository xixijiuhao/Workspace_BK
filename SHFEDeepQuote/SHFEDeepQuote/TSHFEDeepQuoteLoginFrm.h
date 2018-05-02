#pragma once

#include "MBLSpiHandler.h"

class CLoginFrm :public VirtualWnd ,public IQuoteLogin
{
public:
	CLoginFrm()
	{
	}
	void Create(TCHAR* WndName, wstring TitleName);
	

protected:
	virtual LRESULT OnDealMsg(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnCreate();
	virtual LRESULT OnNcDestory(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnBtnClick(WPARAM wParam, LPARAM lParam);
	virtual void QuoteLogin(MBLErrorInfo errprInfo);
private:
	//LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	void GetImgPath(const wchar_t* pszPath);
	void OnQuoteLogin(WPARAM wParam, LPARAM lParam);
	bool CLoginFrm::TestMBLPortConnected();
	
	ULONG_PTR gdiplusStartupToken;
	TStatic m_UserLable;
	TStatic m_PWLable;
	TEdit m_UserEdit;
	TEdit m_PWEdit;

	TCombox				m_comContractType;
    TStaticButton		m_btnTest;

	RECT m_posRect;

	//TCHAR m_szImgName;
	TCHAR  m_szImgFilePath[200];

	int midPositionX;
	int midPositionY;
	//Gdiplus::Image*  m_Img;
public:
	char m_UserName[32];
	char m_Password[32];
	MBLApi* m_pMBLApi;
};