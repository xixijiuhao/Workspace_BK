#pragma once


#include "TradeApi_Type.h"


class TradeConst
{
public:
	static char UnDirect(char AChar)
	{
		if (AChar == dBuy)
			return dSell;
		else
			return dBuy;
	}
	static char UnOffset(char AChar)
	{
		if (AChar == oOpen)
			return oCover;
		else if (AChar == oCover || AChar == oCoverT)
			return oOpen;
		else
			return AChar;
	}

	static bool IsSpread(char CommodityType)
	{
		return (CommodityType == ctSpreadMonth ||	//跨期套利
			CommodityType == ctSpreadCommodity ||	//品种套利
			CommodityType == ctBUL ||				//垂直看涨套利
			CommodityType == ctBER ||				//垂直看跌套利
			CommodityType == ctBLT ||				//水平看涨套利
			CommodityType == ctBRT ||				//水平看跌套利
			CommodityType == ctSTD ||				//跨式套利
			CommodityType == ctSTG 					//宽跨式套利
			);
	}
};