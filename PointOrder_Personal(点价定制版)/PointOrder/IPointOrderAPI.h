#pragma once

const wchar_t PLUG_POINTORDER_NAME[] = L"PointOrder";
const wchar_t PLUG_POINTORDER_VERSION[] = L"1.0.0.1";

class IPointOrderAPI
{
public:
	virtual void CreatePointOrder(POINT Position) = 0;
	virtual void CloseAll() = 0;

};
