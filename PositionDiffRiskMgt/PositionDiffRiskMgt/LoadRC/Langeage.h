#pragma once

enum
{
	IDS_PositionDiff_Risk_Management,	//0 仓差风控
	IDS_Option,							//1期权
	IDS_Contract,						//2合约
	IDS_Position_Range,					//3仓差
	IDS_Add,                //4 添加
	IDS_Remove,             //5 删除
	IDS_Tips,               //6 提示
	IDS_Position_Label,     //7 (|多頭-空頭|)
	IDS_Cancel_Order_Option,//8 撤单选项
	IDS_NotAllow_Cancel,	//9 不允许撤单
	IDS_Cancel_Earliest_One_Order,  //10 撤反向方向的最早一笔可撤挂单
	IDS_Single_Less_Position_Range, //11 开平仓挂单量总和小于仓差
	IDS_Invild_Position_Range,		//12 无效的仓差值
	IDS_CancelTypeSelect,           //13 撤单方式选择

	IDS_Invalid_Contract = 101,     //101 无效合约:%s
	IDS_Invalid_Position,           //102 無效的倉差值
	IDS_Error_Position,             //103 下单后超过仓差风控设置，下单失败
	IDS_Not_Cancel,                 //104 "合约:%s的仓差为%d,委托:%s撤单后将会超过仓差,不允许撤单!\n"
	IDS_More_Cancel,                //105 "合约:%s的仓差为%d,为了保持仓差将会额外撤掉委托:%s.\n"
	IDS_Continue,                   //106 是否继续?			
	IDS_CancelOrderWarning,         //107
	IDS_RestingOrderBeyondLimit,    //108
	IDS_PositionOutofRange,			//109
	IDS_CancelOrderWithConsideOrder,//110
	IDS_Remarks,					//111
	IDS_Remarks1,					//112
	IDS_Remarks2,					//113
	IDS_Remarks3,					//114
	IDS_Remarks4,					//115
	IDS_PositionOutofRangeForbidCancle,					//116
};
