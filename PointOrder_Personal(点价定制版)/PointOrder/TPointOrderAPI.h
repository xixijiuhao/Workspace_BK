#pragma once


class TPointOrderAPI:public IPointOrderAPI
{
public:
	virtual void CreatePointOrder(POINT Position);
	virtual void CloseAll();

};
