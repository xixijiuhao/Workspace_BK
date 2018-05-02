/**
* =====================================================================
*
*       [filename    ] SWRiskMonitor.h
*       [author      ] Hao Lin
*       [Description ]
*       [Timestamp   ] 2017-10-24
*
* ===========Copyright (C) 2002-2017 Esunny. All rights reserved.===========
*/

#ifndef SWRISKMONITOR_H
#define SWRISKMONITOR_H

#include <string>
#include <map>
#include <set>

#include "CommonModule_API.h"
#include "StarApi_API.h"
#include "../share/TradeQuoteString.h"

struct CancelPositionData
{
    int nPositionRange; //�ֲ�
    std::set<int> vecOrderID;
    TContractSingleKey *contract;


    CancelPositionData()
    {
        nPositionRange = 0;
        contract = NULL;
    }
};

struct CancelChangeData
{
    std::set<int> setRemoveID;      //���ܳ�����ID
    std::set<int> setAddID;         //����ί�е�id
    std::set<int> setSrcID;         //���ȷ�ϲ�������Ҫȥ��ԭ����ί��
};

class SWRiskMonitor : ICommonInteractRiskCheck, public ISQuoteEventSpi
{
public:
    SWRiskMonitor();

public:
    //�ֲ��ؼ��
    virtual bool __cdecl PositionRangeCheck(const TSendOrder &TSOrder, ErrorReflectInfo &eError);
    //�����ֲ���
    virtual bool __cdecl CancelPositionRangeCheck(std::set<int> &vecOrderId, char *userNo = "");

public:
    //���ӳɹ�
    virtual void __stdcall OnConnect() {}
    //���ӶϿ�
    virtual void __stdcall OnDisconnect() {}
    //��¼Ӧ��
    virtual void __stdcall OnLogin(const SUserNoType user, const SErrCodeType err) {}
    //��ʼ����ɣ��ڶ��ε�¼���ٳ�ʼ����
    virtual void __stdcall OnInitComplete();

private:
    bool CheckMaxOpen(const TSendOrder &TSOrder, ErrorReflectInfo &eError);
    bool CheckCancel(const TSendOrder &TSOrder, ErrorReflectInfo &eError);
    bool CheckPositionRange(const TSendOrder &TSOrder, ErrorReflectInfo &eError);
    bool CheckPositionSingleKey(const TContractSingleKey &key, ErrorReflectInfo &eError, TDirect direct, const TSendOrder &TSOrder);

    void CalOrderPosition(std::set<int> &orderIdSet, std::map<std::string, CancelPositionData *> &codeMapPosition,
        std::map<int, std::vector<std::string> > &mapOrderIDCmbContract);
    void CalCodePositionRange(const char *userNo, std::map<std::string, CancelPositionData *> &codeMapPosition);
    void CheckCancelPositionRange(const char *userNo, std::map<std::string, CancelPositionData *> &codeMapPosition, std::set<int> &orderIdSet,
        std::map<int, std::vector<std::string> > &mapOrderIDCmbContract, std::map<std::string, CancelChangeData> &mapChangeData);
    std::vector<int> FilterOrder(TDataContainer &tempDataCon, const char *userNo, std::string sCode, TDirect direct);
    void ShowPopInfo(std::map<std::string, CancelChangeData> &mapChangeData, std::set<int> &orderIdSet);

private:
    //��ϱ䵥��
    void CombTContractKeyToTContractSingleKey(const TContractKey &ContractKey, TContractSingleKey &SigCont1, TContractSingleKey &SigCont2);
    //��Ϻ�Լ�Ƿ��Ե�һ��Ϊ׼��1�ǣ�0�Եڶ���Ϊ׼
    bool CombContractIsFirst(const TCommodityKey & CommodityKey);

private:
    bool m_bAuthored;
};

#endif // SWRISKMONITOR_H