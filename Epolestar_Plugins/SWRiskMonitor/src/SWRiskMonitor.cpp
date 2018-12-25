/**
* =====================================================================
*
*       [filename    ] SWRiskMonitor.cpp
*       [author      ] Hao Lin
*       [Description ]
*       [Timestamp   ] 2017-10-24
*
* ===========Copyright (C) 2002-2017 Esunny. All rights reserved.===========
*/

#include "SWRiskMonitor.h"
#include "SWRiskConfigWnd.h"
#include "../share/AnsicUnicode.h"
#include "MainFrame_API.h"
#include "Language.h"
#include "StatusBarWnd.h"
#include "ConfigFrame_API.h"
#include "Authentication_API.h"


#include <cmath>

extern SWRiskConfigWnd *g_cfgWnd;
extern ITradeData *g_pTradeData;
extern ICommonModule *g_pCommonModule;
extern IMainFrame *g_mainframe;
extern StatusBarWnd *g_statusBarWnd;
extern IConfigFrame	*g_conFrame;
extern IAuthent *g_authent;
extern IStarApi	*g_StarApi;
extern ICspAccessApi *g_pCspAccessApi;

#define AUTHENT_KEY             "pd2017"
#define AUTHENT_COMPANY         "SWQH"

const string PASSWORD_Pre = "@ES";

typedef vector<string> vecString;
typedef vector<int> vecInt;

HWND __stdcall GetCfgWnd(const wchar_t *subtitle, const HWND parent)
{
    if (wcscmp(subtitle, LoadResString(IDS_SW_Monitor).c_str()) == 0)
    {
        if (g_cfgWnd)
        {
            if (!IsWindow(g_cfgWnd->GetHwnd()))
            {
                g_cfgWnd->CreateCfgWnd(subtitle);
            }
            return g_cfgWnd->GetHwnd();
        }
    }
    return NULL;
}

SWRiskMonitor::SWRiskMonitor()
    : m_bAuthored(false)
{

}

bool SWRiskMonitor::PositionRangeCheck(const TSendOrder & TSOrder, ErrorReflectInfo & eError)
{
    std::string tCode;
    if (!CConvert::TContractKeyToTContractNo(TSOrder, tCode))
    {
        return true;
    }

    return CheckCancel(TSOrder, eError) && CheckMaxOpen(TSOrder, eError) && CheckPositionRange(TSOrder, eError);
}

bool SWRiskMonitor::CancelPositionRangeCheck(std::set<int>& vecOrderId, char * userNo)
{
    std::map<std::string, std::set<int> > mapUserOrderID;
    if (vecOrderId.size() > 0)
    {
        for (auto it : vecOrderId)
        {
            const TOrder *order = g_pTradeData->GetOrder(it);
            if (!order)
            {
                continue;
            }
            if (mapUserOrderID.find(order->UserNo) == mapUserOrderID.end())
            {
                std::set<int> setOrderID;
                setOrderID.insert(it);
                mapUserOrderID[order->UserNo] = setOrderID;
            }
            else
            {
                mapUserOrderID[order->UserNo].insert(it);
            }
        }
    }
    else
    {
        TDataContainer tempDataCon;
        g_pTradeData->GetParOrder(tempDataCon);

        if (strcmp(userNo, "") != 0)
        {
            for (uint i = 0; i < tempDataCon.size(); i++)
            {
                const TOrder* pOrder = ((const TOrder*)(tempDataCon.at(i)));
                if (!pOrder)
                {
                    continue;
                }

                if (strcmp(pOrder->UserNo, userNo) == 0)
                {
                    if (mapUserOrderID.find(pOrder->UserNo) == mapUserOrderID.end())
                    {
                        std::set<int> setOrderID;
                        setOrderID.insert(pOrder->OrderID);
                        mapUserOrderID[pOrder->UserNo] = setOrderID;
                    }
                    else
                    {
                        mapUserOrderID[pOrder->UserNo].insert(pOrder->OrderID);
                    }
                }
            }
        }
        else
        {
            for (uint i = 0; i < tempDataCon.size(); i++)
            {
                const TOrder* pOrder = ((const TOrder*)(tempDataCon.at(i)));
                if (!pOrder)
                {
                    continue;
                }

                if (mapUserOrderID.find(pOrder->UserNo) == mapUserOrderID.end())
                {
                    std::set<int> setOrderID;
                    setOrderID.insert(pOrder->OrderID);
                    mapUserOrderID[pOrder->UserNo] = setOrderID;
                }
                else
                {
                    mapUserOrderID[pOrder->UserNo].insert(pOrder->OrderID);
                }
            }
        }
    }

    std::map<std::string, CancelPositionData> mapPositionRange;
    std::map<int, vector<std::string> > mapOrderIDCmbContract;//组合合约，key:order id, value:单腿合约名

    std::map<std::string, CancelChangeData> mapChangeData;

    vecOrderId.clear();
    for (auto it : mapUserOrderID)
    {
        mapPositionRange.clear();
        CalOrderPosition(it.second, mapPositionRange, mapOrderIDCmbContract);
        CalCodePositionRange(it.first.c_str(), mapPositionRange);
        CheckCancelPositionRange(it.first.c_str(), mapPositionRange, it.second, mapOrderIDCmbContract, mapChangeData);

        for (auto itID : it.second)
        {
            vecOrderId.insert(itID);
        }
    }
    ShowPopInfo(mapChangeData, vecOrderId);
    return true;
}

void SWRiskMonitor::OnInitComplete()
{
    if (g_authent && !m_bAuthored)
    {
        UserAuthentEx stAuthent;
        memset(&stAuthent, 0, sizeof(UserAuthentEx));
        //if (g_authent->GetAuthent(AUTHENT_COMPANY, AUTHENT_KEY, stAuthent))
        {
            //if (stAuthent.auth.value[0].value && stAuthent.serverdate < stAuthent.auth.value[0].date)
            {
                g_pCommonModule->Regist((ICommonInteractRiskCheck*)this, InteractType_RiskCheck);

                g_conFrame->reg_config_dlg(LoadResString(IDS_SW_Monitor).c_str(), (dlg_create_func)GetCfgWnd, cmtExpand, 15, "esunny.epolestar.configframe");
                g_mainframe->RegStatusBarItemEx(7, g_statusBarWnd->GetHwnd(), 2);
                m_bAuthored = true;

				SConfigSwitchInfo *sInfo = { 0 };
                if (g_pCspAccessApi->GetConfigSwitchInfo(S_CONFIGNO_SWPSW, &sInfo, 1, false) != 0)
                {
                    std::string strDefaultPwd = PASSWORD_Pre + to_string(sInfo->ConfigSwitchValue);
                    g_cfgWnd->SetPassword(strDefaultPwd);
                }
            }
        }
    }

    //SConfigSwitchInfo *sInfo = { 0 };
    //if (g_StarApi->GetConfigSwitchInfo(S_CONFIGNO_SWPSW, &sInfo, 1, false) != 0)
    //{
    //    std::string strDefaultPwd = PASSWORD_Pre + to_string(sInfo->ConfigSwitchValue);
    //    g_cfgWnd->SetPassword(strDefaultPwd);
    //}

    //g_mainframe->RegStatusBarItemEx(8, g_statusBarWnd->GetHwnd(), 2);
    //g_conFrame->reg_config_dlg(LoadResString(IDS_SW_Monitor).c_str(), (dlg_create_func)GetCfgWnd, cmtExpand, 15, "esunny.epolestar.configframe");
    //g_pCommonModule->Regist((ICommonInteractRiskCheck*)this, InteractType_RiskCheck);
}

bool SWRiskMonitor::CheckMaxOpen(const TSendOrder & TSOrder, ErrorReflectInfo & eError)
{
    if (TSOrder.Offset == oOpen)
    {
        if (g_cfgWnd->m_cfgData.m_nMaxOpenQty >= 0)
        {
            int nOpenQty = TSOrder.OrderQty;
            TDataContainer vecOrder;
            g_pTradeData->GetAllOrder(vecOrder, ftValid, TSOrder.Sign);
            for (int i = 0; i < vecOrder.size(); i++)
            {
                const TOrder * order = (const TOrder *)vecOrder.at(i);
                if (order && strcmp(order->UserNo, TSOrder.UserNo) == 0 && order->Offset == oOpen)
                {
                    nOpenQty += order->OrderQty;
                }
            }
            if (nOpenQty > g_cfgWnd->m_cfgData.m_nMaxOpenQty)
            {
                eError.nErrorLevel = EL_Error;
                wcscpy_s(eError.error_text, LoadResString(IDS_Error_Max_Open).c_str());
                return false;
            }
        }
    }
    return true;
}

bool SWRiskMonitor::CheckCancel(const TSendOrder & TSOrder, ErrorReflectInfo & eError)
{
    if (g_cfgWnd->m_cfgData.m_nThreshold > 0)
    {
        const TMoney *pMoney = g_pTradeData->GetMoney(TSOrder.UserNo, "", "CNY", TSOrder.Sign);
        if (pMoney)
        {
            int nThreshold = g_cfgWnd->m_cfgData.m_nThreshold;
            if (nThreshold < (pMoney->PreEquity - pMoney->Equity))
            {
                eError.nErrorLevel = EL_Error;
                wcscpy_s(eError.error_text, LoadResString(IDS_Error_Cancel_Threshold).c_str());
                return false;
            }
        }
    }
    return true;
}

bool SWRiskMonitor::CheckPositionRange(const TSendOrder & TSOrder, ErrorReflectInfo & eError)
{
    std::string tCode;
    if (!CConvert::TContractKeyToTContractNo(TSOrder, tCode))
    {
        return true;
    }
    if (TSOrder.CommodityType == ctSpreadCommodity || TSOrder.CommodityType == ctSpreadMonth)
    {
        TContractSingleKey key[2];
        memset(&key, 0, sizeof(TContractSingleKey) * 2);
        CombTContractKeyToTContractSingleKey(TSOrder, key[0], key[1]);

        return CheckPositionSingleKey(key[0], eError, TSOrder.Direct, TSOrder) &&
            CheckPositionSingleKey(key[1], eError, TSOrder.Direct == dBuy ? dSell : dBuy, TSOrder);
    }
    else
    {
        return CheckPositionSingleKey(TSOrder, eError, TSOrder.Direct, TSOrder);
    }
    /*if (g_cfgWnd->m_cfgData.m_mapPositonRange.find(tCode) != g_cfgWnd->m_cfgData.m_mapPositonRange.end())
    {
        int nRange = g_cfgWnd->m_cfgData.m_mapPositonRange[tCode];
        if (nRange <= 0)
        {
            return true;
        }
        
        int nBuyCount = 0, nSellCount = 0;

        const TPosition *buyPositon = g_pTradeData->GetPositionTotal(TSOrder.UserNo, &TSOrder, dBuy);
        if (buyPositon)
        {
            nBuyCount = buyPositon->PositionQty;
        }
        const TPosition *sellPositon = g_pTradeData->GetPositionTotal(TSOrder.UserNo, &TSOrder, dSell);
        if (sellPositon)
        {
            nSellCount = sellPositon->PositionQty;
        }

        const TOrderQty *orderQty = g_pTradeData->GetParOrderQty(TSOrder.UserNo, &TSOrder);

        if (orderQty)
        {
            nBuyCount = nBuyCount + orderQty->BuyOpenQty - orderQty->SellCoverQty - orderQty->SellCoverDayQty;
            if (nBuyCount < 0)
            {
                nBuyCount = 0;
            }
            nSellCount = nSellCount + orderQty->SellOpenQty - orderQty->BuyCoverDayQty - orderQty->BuyCoverQty;
            if (nSellCount < 0)
            {
                nSellCount = 0;
            }
        }

        if (TSOrder.Direct == dBuy)
        {
            if (TSOrder.Offset == oOpen)
            {
                nBuyCount += TSOrder.OrderQty;
            }
            else
            {
                nSellCount -= TSOrder.OrderQty;
            }
        }
        if (TSOrder.Direct == dSell)
        {
            if (TSOrder.Offset == oOpen)
            {
                nSellCount += TSOrder.OrderQty;
            }
            else
            {
                nBuyCount -= TSOrder.OrderQty;
            }
        }
        if (abs(nBuyCount - nSellCount) > nRange)
        {
            eError.nErrorLevel = EL_Error;
            wcscpy_s(eError.error_text, LoadResString(IDS_Error_Position).c_str());
            return false;
        }
    }
    return true;*/
}

bool SWRiskMonitor::CheckPositionSingleKey(const TContractSingleKey & key, ErrorReflectInfo & eError, TDirect direct, const TSendOrder &TSOrder)
{
    std::string tCode;
    if (!CConvert::PosContractToStr(tCode, key))
    {
        return true;
    }

    if (g_cfgWnd->m_cfgData.m_mapPositonRange.find(tCode) != g_cfgWnd->m_cfgData.m_mapPositonRange.end())
    {
        int nRange = g_cfgWnd->m_cfgData.m_mapPositonRange[tCode];
        if (nRange <= 0)
        {
            return true;
        }

        int nBuyCount = 0, nSellCount = 0;

        const TPosition *buyPositon = g_pTradeData->GetPositionTotal(TSOrder.UserNo, &key, dBuy);
        if (buyPositon)
        {
            nBuyCount = buyPositon->PositionQty;
        }
        const TPosition *sellPositon = g_pTradeData->GetPositionTotal(TSOrder.UserNo, &key, dSell);
        if (sellPositon)
        {
            nSellCount = sellPositon->PositionQty;
        }

        const TOrderQty *orderQty = g_pTradeData->GetParOrderQty(TSOrder.UserNo, &key);

        if (orderQty)
        {
            nBuyCount = nBuyCount + orderQty->BuyOpenQty - orderQty->SellCoverQty - orderQty->SellCoverDayQty;
            if (nBuyCount < 0)
            {
                nBuyCount = 0;
            }
            nSellCount = nSellCount + orderQty->SellOpenQty - orderQty->BuyCoverDayQty - orderQty->BuyCoverQty;
            if (nSellCount < 0)
            {
                nSellCount = 0;
            }
        }

        if (direct == dBuy)
        {
            if (TSOrder.Offset == oOpen)
            {
                nBuyCount += TSOrder.OrderQty;
            }
            else
            {
                nSellCount -= TSOrder.OrderQty;
            }
        }
        if (direct == dSell)
        {
            if (TSOrder.Offset == oOpen)
            {
                nSellCount += TSOrder.OrderQty;
            }
            else
            {
                nBuyCount -= TSOrder.OrderQty;
            }
        }
        if (abs(nBuyCount - nSellCount) > nRange)
        {
            eError.nErrorLevel = EL_Error;
            wcscpy_s(eError.error_text, LoadResString(IDS_Error_Position).c_str());
            return false;
        }
    }
    return true;
}

void SWRiskMonitor::CalOrderPosition(std::set<int>& orderIdSet, std::map<std::string, CancelPositionData>& codeMapPosition, 
    std::map<int, std::vector<std::string> > &mapOrderIDCmbContract)
{
    for (auto it : orderIdSet)
    {
        const TOrder *order = g_pTradeData->GetOrder(it);
        if (!order)
        {
            continue;
        }
        std::string tCode;
        if (!CConvert::TContractKeyToTContractNo(*order, tCode))
        {
            continue;
        }

        if (order->CommodityType == ctSpreadCommodity || order->CommodityType == ctSpreadMonth)
        {
            TContractSingleKey key[2];
            memset(&key, 0, sizeof(TContractSingleKey) * 2);
            CombTContractKeyToTContractSingleKey(*order, key[0], key[1]);

            bool bFirst = false;  //第一腿有仓差设置
            std::string tCode;
            if (CConvert::PosContractToStr(tCode, key[0]))
            {
                if (g_cfgWnd->m_cfgData.m_mapPositonRange.find(tCode) != g_cfgWnd->m_cfgData.m_mapPositonRange.end())
                {
                    if (codeMapPosition.find(tCode) == codeMapPosition.end())
                    {
                        CancelPositionData data;
                        codeMapPosition[tCode] = data;
                    }

                    if (order->Direct == dBuy)
                    {
                        codeMapPosition[tCode].nPositionRange += order->OrderQty;
                    }
                    else
                    {
                        codeMapPosition[tCode].nPositionRange -= order->OrderQty;
                    }

                    codeMapPosition[tCode].vecOrderID.insert(it);
                    codeMapPosition[tCode].contract = &key[0];
                    bFirst = true;
                }
            }

            std::string tCode1;
            if (CConvert::PosContractToStr(tCode1, key[1]))
            {
                if (g_cfgWnd->m_cfgData.m_mapPositonRange.find(tCode1) != g_cfgWnd->m_cfgData.m_mapPositonRange.end())
                {
                    if (codeMapPosition.find(tCode1) == codeMapPosition.end())
                    {
                        CancelPositionData data;
                        codeMapPosition[tCode1] = data;
                    }

                    if (order->Direct == dBuy)
                    {
                        codeMapPosition[tCode].nPositionRange -= order->OrderQty;
                    }
                    else
                    {
                        codeMapPosition[tCode].nPositionRange += order->OrderQty;
                    }

                    codeMapPosition[tCode].vecOrderID.insert(it);
                    codeMapPosition[tCode].contract = &key[1];
                    if (bFirst)
                    {
                        vector<string> vecCode;
                        vecCode.push_back(tCode);
                        vecCode.push_back(tCode1);
                        mapOrderIDCmbContract[it] = vecCode;
                    }
                }
            }
        }
        else
        {
            std::string tCode;
            if (!CConvert::PosContractToStr(tCode, *order))
            {
                continue;
            }

            if (g_cfgWnd->m_cfgData.m_mapPositonRange.find(tCode) != g_cfgWnd->m_cfgData.m_mapPositonRange.end())
            {
                if (codeMapPosition.find(tCode) == codeMapPosition.end())
                {
                    CancelPositionData data;
                    codeMapPosition[tCode] = data;
                }

                if (order->Direct == dBuy)
                {
                    codeMapPosition[tCode].nPositionRange += order->OrderQty;
                }
                else
                {
                    codeMapPosition[tCode].nPositionRange -= order->OrderQty;
                }

                codeMapPosition[tCode].vecOrderID.insert(it);
                codeMapPosition[tCode].contract = (TContractSingleKey *)order;
            }
        }
    }
}

void SWRiskMonitor::CalCodePositionRange(const char * userNo, std::map<std::string, CancelPositionData>& codeMapPosition)
{
    for (auto it : codeMapPosition)
    {
        int nBuyCount = 0, nSellCount = 0;

        const TPosition *buyPositon = g_pTradeData->GetPositionTotal(userNo, it.second.contract, dBuy);
        if (buyPositon)
        {
            nBuyCount = buyPositon->PositionQty;
        }
        const TPosition *sellPositon = g_pTradeData->GetPositionTotal(userNo, it.second.contract, dSell);
        if (sellPositon)
        {
            nSellCount = sellPositon->PositionQty;
        }

        const TOrderQty *orderQty = g_pTradeData->GetParOrderQty(userNo, it.second.contract);

        if (orderQty)
        {
            nBuyCount = nBuyCount + orderQty->BuyOpenQty - orderQty->SellCoverQty - orderQty->SellCoverDayQty;
            nSellCount = nSellCount + orderQty->SellOpenQty - orderQty->BuyCoverDayQty - orderQty->BuyCoverQty;
        }

        it.second.nPositionRange = (nBuyCount - nSellCount) - it.second.nPositionRange;//撤单后的买仓-卖仓
    }
}

void SWRiskMonitor::CheckCancelPositionRange(const char *userNo, std::map<std::string, CancelPositionData>& codeMapPosition, std::set<int> &orderIdSet,
    std::map<int, std::vector<std::string> > &mapOrderIDCmbContract, std::map<std::string, CancelChangeData> &mapChangeData)
{
    std::set<std::string> setChecked;                       //已检查过的合约

    TDataContainer tempDataCon;
    g_pTradeData->GetParOrder(tempDataCon);

    bool bAllChecked = false;
    while (!bAllChecked)
    {
        int i = 0;
        for (auto it : codeMapPosition)
        {
            if (setChecked.find(it.first) != setChecked.end())
            {//checked
                continue;
            }

            if (g_cfgWnd->m_cfgData.m_mapPositonRange.find(it.first) != g_cfgWnd->m_cfgData.m_mapPositonRange.end())
            {
                int nRange = g_cfgWnd->m_cfgData.m_mapPositonRange[it.first];
                if (nRange <= 0)
                {
                    continue;
                }

                if (abs(it.second.nPositionRange) > nRange)
                {//仓差超限
                    const TOrderQty *orderQty = g_pTradeData->GetParOrderQty(userNo, it.second.contract);

                    if (mapChangeData.find(it.first) == mapChangeData.end())
                    {
                        CancelChangeData data;
                        mapChangeData[it.first] = data;
                    }

                    int nAddRange = abs(it.second.nPositionRange) - nRange; //补仓差
                    if (orderQty && (
                        (it.second.nPositionRange > 0 && orderQty->BuyQty >= nAddRange)//买方向挂单数量够减小仓差至有效范围
                        || (it.second.nPositionRange < 0 && orderQty->SellQty >= nAddRange))//卖方向挂单数量够减小仓差至有效范围
                        )
                    {
                        TDirect direct = it.second.nPositionRange > 0 ? dBuy : dSell;
                        vecInt vecOrder = FilterOrder(tempDataCon, userNo, it.first, direct);

                        int nSum = 0;
                        for (int i = 0; i < vecOrder.size(); i++)
                        {
                            mapChangeData[it.first].setAddID.insert(vecOrder.at(i));

                            const TOrder *pOrder = g_pTradeData->GetOrder(vecOrder.at(i));
                            if (pOrder)
                            {
                                nSum += pOrder->OrderQty;
                            }
                            if (nSum >= nAddRange)
                            {
                                break;
                            }
                        }
                        for (auto itID : it.second.vecOrderID)
                        {
                            mapChangeData[it.first].setSrcID.insert(itID);
                        }
                        
                    }
                    else
                    {//挂单不足，不能撤单
                        bool bReCheck = false;

                        
                        for (auto itID : it.second.vecOrderID)
                        {
                            mapChangeData[it.first].setRemoveID.insert(itID);
                            //如果是组合合约，需要重新检查另一腿合约
                            if (mapOrderIDCmbContract.find(itID) != mapOrderIDCmbContract.end())
                            {
                                vector<string> vecCode = mapOrderIDCmbContract[itID];
                                for (auto itCode : vecCode)
                                {
                                    if (itCode != it.first && codeMapPosition.find(itCode) != codeMapPosition.end())
                                    {//另一腿合约也有仓差设置
                                        const TOrder *order = g_pTradeData->GetOrder(itID);
                                        if (order)
                                        {
                                            if (order->Direct == dBuy)
                                            {
                                                codeMapPosition[itCode].nPositionRange += order->OrderQty;
                                            }
                                            else
                                            {
                                                codeMapPosition[itCode].nPositionRange -= order->OrderQty;
                                            }
                                        }
                                        if (setChecked.find(itCode) != setChecked.end())
                                        {
                                            setChecked.erase(itCode);
                                            mapChangeData.erase(itCode);

                                            bReCheck = true;
                                        }
                                    }
                                }

                            }
                        }

                        if (bReCheck)
                        {
                            break;
                        }
                    }
                    
                }
            }

            i++;
        }

        if (i == codeMapPosition.size())
        {
            bAllChecked = true;
        }
    }
}

std::vector<int> SWRiskMonitor::FilterOrder(TDataContainer & tempDataCon, const char * userNo, std::string sCode, TDirect direct)
{
    vector<int> vecOrderID;

    for (uint i = 0; i < tempDataCon.size(); i++)
    {
        const TOrder* pOrder = ((const TOrder*)(tempDataCon.at(i)));
        if (!pOrder)
        {
            continue;
        }
        if (strcmp(pOrder->UserNo, userNo) != 0)
        {
            continue;
        }
        if (pOrder->Direct != direct)
        {
            continue;
        }
        std::string tCode;
        if (!CConvert::PosContractToStr(tCode, *pOrder))
        {
            continue;
        }

        if (tCode == sCode)
        {
            vecOrderID.push_back(pOrder->OrderID);
        }
    }
    sort(vecOrderID.begin(), vecOrderID.end());
    return vecOrderID;
}

void SWRiskMonitor::ShowPopInfo(std::map<std::string, CancelChangeData>& mapChangeData, std::set<int>& orderIdSet)
{
    wstring strRemoveInfo = L"", strAddInfo = L"";
    wchar_t strTemp[1500] = { 0 };
    wstring strAddID = L"", strRemoveID = L"";
    //提示信息
    for (auto it : mapChangeData)
    {
        strAddID = L"";
        strRemoveID = L"";
        for (auto itID : it.second.setRemoveID)
        {
            strRemoveID += to_wstring(itID) + L" ";
            orderIdSet.erase(itID);
        }
        if (it.second.setRemoveID.size() > 0)
        {
            swprintf_s(strTemp, (LoadResString(IDS_Not_Cancel) + L"\n").c_str(), CConvert::UTF8ToUnicode(it.first).c_str(),
                g_cfgWnd->m_cfgData.m_mapPositonRange[it.first], strRemoveID.c_str());
            strRemoveInfo += strTemp;
        }

        for (auto itID : it.second.setAddID)
        {
            strAddID += to_wstring(itID) + L" ";
        }
        if (it.second.setAddID.size() > 0)
        {
            swprintf_s(strTemp, (LoadResString(IDS_More_Cancel) + L"\n").c_str(), CConvert::UTF8ToUnicode(it.first).c_str(),
                g_cfgWnd->m_cfgData.m_mapPositonRange[it.first], strAddID.c_str());
            strAddInfo += strTemp;
        }

    }

    if (!strRemoveInfo.empty())
    {
        MessageBox(NULL, strRemoveInfo.c_str(), LoadResString(IDS_Tips).c_str(), NULL);
    }

    if (!strAddInfo.empty())
    {
        if (MessageBox(NULL, strAddInfo.c_str(), LoadResString(IDS_Tips).c_str(), MB_YESNO) == IDYES)
        {//继续撤单，添加反向委托撤单
            for (auto it : mapChangeData)
            {
                for (auto itID : it.second.setAddID)
                {
                    orderIdSet.insert(itID);
                }
            }
        }
        else
        {//取消撤单，去掉原来要删除的委托
            for (auto it : mapChangeData)
            {
                for (auto itID : it.second.setSrcID)
                {
                    orderIdSet.erase(itID);
                }
            }
        }
    }
}


//组合合约是否以第一腿为准，1是，0以第二腿为准
bool SWRiskMonitor::CombContractIsFirst(const TCommodityKey & CommodityKey)
{
    const SCommodity *pCommodity = g_pTradeData->GetCommodity(&CommodityKey);
    if (!pCommodity)	return true;
    if (dfFirst == pCommodity->SpreadDirect)
        return true;
    else if (dfSecond == pCommodity->SpreadDirect)
        return false;
    else
        return true;
}

void SWRiskMonitor::CombTContractKeyToTContractSingleKey(const TContractKey &ContractKey, TContractSingleKey &SigCont1, TContractSingleKey &SigCont2)
{
    const SCommodity *pCommodity = g_pTradeData->GetCommodity(&ContractKey);
    if (!pCommodity)	return;
    bool bFirstDirect = CombContractIsFirst(ContractKey);
    if (bFirstDirect)
    {
        //1		
        if (pCommodity->TargetCommodity[0])
            CConvert::SCommodityNoToTCommodityKey(pCommodity->TargetCommodity[0]->CommodityNo, SigCont1);
        strcpy_s(SigCont1.Sign, ContractKey.Sign);
        strcpy_s(SigCont1.ContractNo, ContractKey.ContractNo);
        strcpy_s(SigCont1.StrikePrice, ContractKey.StrikePrice);
        strcpy_s(SigCont1.ExchangeNo, ContractKey.ExchangeNo);
        SigCont1.OptionType = ContractKey.OptionType;
        //2	
        if (ContractKey.CommodityType == ctSpreadMonth && pCommodity->TargetCommodity[0])
        {
            CConvert::SCommodityNoToTCommodityKey(pCommodity->TargetCommodity[0]->CommodityNo, SigCont2);
        }
        else if (pCommodity->TargetCommodity[1])
            CConvert::SCommodityNoToTCommodityKey(pCommodity->TargetCommodity[1]->CommodityNo, SigCont2);
        strcpy_s(SigCont2.Sign, ContractKey.Sign);
        strcpy_s(SigCont2.ContractNo, ContractKey.ContractNo2);
        strcpy_s(SigCont2.StrikePrice, ContractKey.StrikePrice2);
        strcpy_s(SigCont2.ExchangeNo, ContractKey.ExchangeNo);
        SigCont2.OptionType = ContractKey.OptionType2;
    }
    else//以第二腿为准
    {
        //1		
        if (ContractKey.CommodityType == ctSpreadMonth && pCommodity->TargetCommodity[0])
        {
            CConvert::SCommodityNoToTCommodityKey(pCommodity->TargetCommodity[0]->CommodityNo, SigCont1);
        }
        else if (pCommodity->TargetCommodity[1])
            CConvert::SCommodityNoToTCommodityKey(pCommodity->TargetCommodity[1]->CommodityNo, SigCont1);
        strcpy_s(SigCont1.Sign, ContractKey.Sign);
        strcpy_s(SigCont1.ContractNo, ContractKey.ContractNo2);
        strcpy_s(SigCont1.StrikePrice, ContractKey.StrikePrice2);
        strcpy_s(SigCont1.ExchangeNo, ContractKey.ExchangeNo);
        SigCont1.OptionType = ContractKey.OptionType2;
        //2		
        if (pCommodity->TargetCommodity[0])
            CConvert::SCommodityNoToTCommodityKey(pCommodity->TargetCommodity[0]->CommodityNo, SigCont2);
        strcpy_s(SigCont2.Sign, ContractKey.Sign);
        strcpy_s(SigCont2.ContractNo, ContractKey.ContractNo);
        strcpy_s(SigCont2.StrikePrice, ContractKey.StrikePrice);
        strcpy_s(SigCont2.ExchangeNo, ContractKey.ExchangeNo);
        SigCont2.OptionType = ContractKey.OptionType;
    }
}
