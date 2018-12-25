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
		return (CommodityType == ctSpreadMonth ||	//��������
			CommodityType == ctSpreadCommodity ||	//Ʒ������
			CommodityType == ctBUL ||				//��ֱ��������
			CommodityType == ctBER ||				//��ֱ��������
			CommodityType == ctBLT ||				//ˮƽ��������
			CommodityType == ctBRT ||				//ˮƽ��������
			CommodityType == ctSTD ||				//��ʽ����
			CommodityType == ctSTG 					//���ʽ����
			);
	}
};