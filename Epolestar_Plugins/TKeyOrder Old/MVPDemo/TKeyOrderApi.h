#ifndef _TKEY_ORDER_API_H_
#define _TKEY_ORDER_API_H_

const wchar_t PLUG_TKEYORDER_NAME[] = L"TKeyOrder";
const wchar_t PLUG_TKEYORDER_VERSION[] = L"1.0.0.1";

#define TKeyViewShow  1
#define TKeyViewHide  2
class TKeyOrderApi
{
public:
	//1=show 2=hide
	virtual void Show(int iShow = TKeyViewShow) = 0;
};

class TKeyOrder:public TKeyOrderApi
{
public:
	void Show(int iShow = TKeyViewShow);
};

#endif