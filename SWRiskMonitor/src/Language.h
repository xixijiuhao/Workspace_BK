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
    IDS_SW_Monitor,         //0 ������
    IDS_Option,             //1 ��Ȩ
    IDS_Contract,           //2 ��Լ
    IDS_Position_Range,     //3 �ֲ�
    IDS_Add,                //4 ���
    IDS_Remove,             //5 ɾ��
    IDS_Threshold,          //6 ��ֵ
    IDS_Tips,               //7 ��ʾ
    IDS_Max_Open_Qty,       //8 ��󿪲���
    IDS_Position_Label,     //9 (|���^-���^|)
    IDS_Cancel_Label,       //10 (��Ȩ��-��Ȩ��)
    IDS_Position_Monitor,   //11 �ֲ���
    IDS_Cancel_Monitor,     //12 �س����
    IDS_Open_Monitor,       //13 ���ִ������
    IDS_Password,           //14 ����
    IDS_Check_Password,     //15 У������

    IDS_Invalid_Contract = 101,     //101 ��Ч��Լ:%s
    IDS_Invalid_Position,           //102 �oЧ�Ă}��ֵ
    IDS_Error_Max_Open,             //103 �Ѵﵽ��󿪲���,�������ٿ���
    IDS_Error_Cancel_Threshold,     //104 Ȩ����ٳ����س�ֵ��ֻ�ܽ��г�������
    IDS_Error_Position,             //105 �µ��󳬹��ֲ������ã��µ�ʧ��
    IDS_Error_Password,             //106 �������
    IDS_Not_Cancel,                 //107 "��Լ:%s�Ĳֲ�Ϊ%d,ί��:%s�����󽫻ᳬ���ֲ�,��������!\n"
    IDS_More_Cancel,                //108 "��Լ:%s�Ĳֲ�Ϊ%d,Ϊ�˱��ֲֲ����⳷��ί��:%s.\n"
    IDS_Continue,                   //109 �Ƿ����?
};

#endif // LANGUAGE_H