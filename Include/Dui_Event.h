#ifndef _DUI_EVENT_
#define _DUI_EVENT_

//button
class TDuiButtonControl;
class TDuiButtonControlSpi
{
public:
	virtual void OnButtonClick(TDuiButtonControl* obj) = 0;
};

//popmenu
struct TDuiPopMenuItem;
class TDuiPopMenuControlSpi
{
public:
	virtual void __cdecl OnPopMenuClick(TDuiPopMenuItem* obj) = 0;
};

//combo
class TDuiComboSpi
{
public:
	virtual void OnComboSelectChg(int index) = 0;
};

//listview
class TDuiListViewControl;
class TDuiListViewControlSpi
{
public:
	virtual void OnListViewClick(TDuiListViewControl* obj) = 0;
	virtual void OnListViewDbClick(TDuiListViewControl* obj) = 0;
};



#endif