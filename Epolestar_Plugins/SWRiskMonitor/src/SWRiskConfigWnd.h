/**
* =====================================================================
*
*       [filename    ] SWRiskConfigWnd.h
*       [author      ] Hao Lin
*       [Description ]
*       [Timestamp   ] 2017-10-13
*
* ===========Copyright (C) 2002-2017 Esunny. All rights reserved.===========
*/

#ifndef SWRISKCONFIGWND_H
#define SWRISKCONFIGWND_H

#include <map>
#include <set>
#include <vector>

#include "IxFrm.h"
#include "../ctl/CxEdit.h"
#include "../ctl/CxButton.h"
#include "../ctl/CxStatic.h"
#include "../share/popwincode.h"
#include "../share/IOperator.h"
#include "StarApi_API.h"


enum ControlID
{
    ID_Begin,
    ID_Static_Password,
    ID_Edit_Password,
    ID_Button_Check_Pwd,
    ID_Static_Cancel,
    ID_Edit_Cancel,
    ID_Spin_Cancel,
    ID_Static_Cancel_Label,
    ID_Static_Max_Open,
    ID_Edit_Max_Open,
    ID_Spin_Max_Open,
    ID_Static_Contract,
    ID_Edit_Contract,
    ID_Button_Select_Contract,
    ID_Static_Position_Minus,
    ID_Edit_Position_Minus,
    ID_Static_Position_Label,
    ID_Button_Add,
    ID_Button_Delete,
    ID_ListView_Contract,
    ID_Groupbox_Position,
    ID_Groupbox_Cancel,
    ID_Groupbox_Open,
    ID_End,
};

struct ConfigData
{
    int m_nMaxOpenQty;      //最大开仓量
    int m_nThreshold;       //撤单阈值
    std::map<std::string, int> m_mapPositonRange;   //仓差

    ConfigData()
    {
        m_nMaxOpenQty = -1;
        m_nThreshold = -1;
    }
};

class SWRiskConfigWnd : public TIxFrm, public IOperator
{
public:
    SWRiskConfigWnd();
    
    void CreateCfgWnd(const wchar_t *title);

    virtual void SetContractNo(string sCode);

    void SetPassword(const std::string strPwd);

public:
    WNDPROC m_procListview;

protected:
    virtual	LRESULT	WndProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
    void OnCreate();
    void OnCommand(WPARAM wParam, LPARAM lParam);
    void OnPaint();
    void OnNotify(WPARAM wParam, LPARAM lParam);

    void OnBtnSelect();
    void OnBtnAdd();
    void OnBtnDelete();
    void OnBtnCheckPwd();

private:
    void InitListView();
    void SaveConfig();
    void LoadConfig();
    void UpdateListView();
    void EnableCtrl(bool bEnable);

private:
    HWND m_hControl[ID_End - ID_Begin + 1];
    HBRUSH m_hStaticBKBrush;		//静态文本背景画刷

    CxButton	m_AddBtn;
    CxButton	m_DeleteBtn;
    CxButton    m_SelectBtn;
    CxButton    m_CheckPwdBtn;

    CxEdit		m_EditPosition;
    CxEdit      m_EditOpen;
    CxEdit      m_EditCancel;
    CxEdit      m_EditContract;
    CxEdit      m_EditPassword;

public:
    ConfigData m_cfgData;

    CPopWinCode m_popWinCode;

    bool m_bCheckedPwd;
    std::string m_password;
};

#endif // SWRISKCONFIGWND_H