/*************************************************************************
* 
*           [File        ] Language.h
*           [author      ] Hao Lin
*           [Description ] 
*           [Timestamp   ] 2017-10-13
*
*           Copyright (C) 2017 Esunny. All rights reserved.
*
* ***********************************************************************/

#ifndef LANGUAGE_H
#define LANGUAGE_H

enum
{
    IDS_SW_Monitor,         //0 申万风控
    IDS_Option,             //1 期权
    IDS_Contract,           //2 合约
    IDS_Position_Range,     //3 仓差
    IDS_Add,                //4 添加
    IDS_Remove,             //5 删除
    IDS_Threshold,          //6 阈值
    IDS_Tips,               //7 提示
    IDS_Max_Open_Qty,       //8 最大开仓数
    IDS_Position_Label,     //9 (|多頭-空頭|)
    IDS_Cancel_Label,       //10 (今权益-昨权益)
    IDS_Position_Monitor,   //11 仓差风控
    IDS_Cancel_Monitor,     //12 回撤风控
    IDS_Open_Monitor,       //13 开仓次数风控
    IDS_Password,           //14 密码
    IDS_Check_Password,     //15 校验密码

    IDS_Invalid_Contract = 101,     //101 无效合约:%s
    IDS_Invalid_Position,           //102 無效的倉差值
    IDS_Error_Max_Open,             //103 已达到最大开仓数,不允许再开仓
    IDS_Error_Cancel_Threshold,     //104 权益减少超过回撤值，只能进行撤单操作
    IDS_Error_Position,             //105 下单后超过仓差风控设置，下单失败
    IDS_Error_Password,             //106 密码错误
    IDS_Not_Cancel,                 //107 "合约:%s的仓差为%d,委托:%s撤单后将会超过仓差,不允许撤单!\n"
    IDS_More_Cancel,                //108 "合约:%s的仓差为%d,为了保持仓差将会额外撤掉委托:%s.\n"
    IDS_Continue,                   //109 是否继续?
};

#endif // LANGUAGE_H