enum
{
	IDS_STRING_Error,								//"0", "提示"
	IDS_STRING_DPLastPrice,							//"1", "最新超价"
	IDS_STRING_DPOppoPrice,							//"2", "对盘超价"
	IDS_STRING_DPQuenePrice,						//"3", "挂单超价"
	IDS_STRING_DPNoPrice,							//"4", "不默认"
	IDS_STRING_Speculate,							//"5", "投机"
	IDS_STRING_Hedge,								//"6", "保值"
	IDS_STRING_EmptyPLast,							//"7", "最新价"
	IDS_STRING_EmptyPMarket,						//"8", "市价"
	IDS_STRING_EmptyPUpDownLimit,					//"9", "反向停板"
	IDS_STRING_EmptyPOppo,							//"10", "对盘超价"
	IDS_STRING_EmptyPQuene,							//"11", "挂单超价"
	IDS_STRING_OrderTypeLimitKey,					//"12", "限价"
	IDS_STRING_OrderTypeMarketKey,					//"13", "市价"
	IDS_STRING_OrderTypeLStopKey,					//"14", "限价止损"
	IDS_STRING_OrderTypeMStopKey,					//"15", "止损"
	IDS_STRING_OrderTypeIceKey,						//"16", "冰山"
	IDS_STRING_OrderTypeGhostKey,					//"17", "影子"
	IDS_STRING_KeyboardBigFont,						//"18", "大字体"
	IDS_STRING_KeyboardMidFont,						//"19", "中字体"
	IDS_STRING_KeyboardSmallFont,					//"20", "小字体"
	IDS_STRING_MarketLevelNoLimit,					//"21", "不限"
	IDS_STRING_MarketLevelOne,						//"22", "一档"
	IDS_STRING_MarketLevelFive,						//"23", "五档"
	IDS_STRING_SongTiFont,							//"24", "宋体"
	IDS_STRING_TJTriggerModeLast,					//"25", "最新价"
	IDS_STRING_TJTriggerModeBuy,					//"26", "买价"
	IDS_STRING_TJTriggerModeSell,					//"27", "卖价"
	IDS_STRING_TJTriggerModeLoss,					//"28", "止损模式"
	IDS_STRING_TJTriggerModePro,					//"29", "止赢模式"
	IDS_STRING_TJTriggerPriceLast,					//"30", "最新超价"
	IDS_STRING_TJTriggerPriceOppo,					//"31", "对盘超价"
	IDS_STRING_TJTriggerPriceQuene,					//"32", "挂单超价"
	IDS_STRING_TJTriggerPriceNo,					//"33", "不默认"
	IDS_STRING_ChangeCode,							//"34", "合约代码"
	IDS_STR_ModifyOrder,							//"35", "修改"
	IDS_STR_Cancel,									//"36", "取消"
	IDS_STR_DoOrder,								//"37", "确定(Y)"
	IDS_STR_Seq,									//"38", "顺序"
	IDS_Direct_Buy,									//"39", "买入"
	IDS_Direct_Sell,								//"40", "卖出"
	IDS_Offset_Open,								//"41", "开仓"
	IDS_Offset_CoverT,								//"42", "平今"
	IDS_Offset_Cover,								//"43", "平仓"
	IDS_STR_ValidType_FOK,							//"44", "即时全部"
	IDS_STR_ValidType_IOC,							//"45", "即时部分"
	IDS_STR_ValidType_GFD,							//"46", "当日有效"
	IDS_STR_ValidType_GTC,							//"47", "长期有效"
	IDS_STR_ValidType_GTD,							//"48", "限期有效"
	IDS_STR_ValidType_GFS,							//"49", "本节有效"
	IDS_STR_TirgMode_LastSpace,						//"50", "最新价 "
	IDS_STR_TirgMode_Last,							//"51", "最新价"
	IDS_STR_TirgMode_Buy,							//"52", "买价"
	IDS_STR_TirgMode_Sell,							//"53", "卖价"
	IDS_STR_STATIC_Code,							//"54", "合约代码:"
	IDS_STR_STATIC_Direct,							//"55", "买/卖"
	IDS_STR_STATIC_Offset,							//"56", "开/平"
	IDS_STR_STATIC_OrderPrice,						//"57", "委托价格"
	IDS_STR_STATIC_OrderQty,						//"58", "委托数量"
	IDS_STR_STATIC_MultiQty,						//"59", "数量倍率"
	IDS_STR_STATIC_TrigMode,						//"60", "触发方式"
	IDS_STR_STATIC_TrigCon,							//"61", "条件"
	IDS_STR_STATIC_TrigPrice,						//"62", "触发价格"
	IDS_STR_QtyCount,								//"63", "下单次数"
	IDS_STR_IncreQty,								//"64", "数量增量"
	IDS_STR_IncrePrice,								//"65", "价格增量"
	IDS_STR_STATIC_OrderType,						//"66", "定单类型"
	IDS_STR_STATIC_ValidType,						//"67", "有效类型"
	IDS_STR_STATIC_ValidDate,						//"68", "有效日期"
	IDS_STR_STATIC_MaxQty,							//"69", "最大量"
	IDS_STR_STATIC_MinQty,							//"70", "最小量"
	IDS_STR_STATIC_UserNo,							//"71", "资金账号"
	IDS_STR_STATIC_BuyPos,							//"72", "多头：888"
	IDS_STR_STATIC_SellPos,							//"73", "空头：888"
	IDS_STR_BatchOrder,								//"74", "批量&U"
	IDS_STR_PayOrder,								//"75", "埋单(P)"
	IDS_STR_AutoOrder,								//"76", "自动单"
	IDS_STRING_RegMenu,								//"77", "横向下单"
	IDS_STR_STATIC_PLGroupNo,						//"78", "批量组号"
	IDS_STR_InvalidCode,							//"79", "输入合约无效 !"
	IDS_STR_InvalidOffset,							//"80", "开平输入无效"
	IDS_STR_CanOrderFore,							//"81", "%d(对冲:%d 建仓:%d)"
	IDS_STR_PreDepositFore,							//"82", "预冻结:基币 %0.*lf(%s %0.*lf)"
	IDS_STR_CanOpenCover,							//"83", "可开:%d 可平:%d"
	IDS_STR_CanOpenCoverT,							//"84", "可开:%d 可平今:%d 可平昨:%d"
	IDS_STR_InvalidaQuot,							//"85", "合约行情失败！"
	IDS_STR_Get,									//"86", "取"
	IDS_STR_SuperQueProtect,						//"87", "超价挂单保护,"
	IDS_STR_BuyPrice,								//"88", "买入价格"
	IDS_STR_SellPrice,								//"89", "卖出价格"
	IDS_STR_InvalidQuotCode,						//"90", "无效的行情合约代码！"
	IDS_STR_InvalidTradeCode,						//"91", "输入合约无效 !"
	IDS_STR_NoSet,									//"92", "未设置"
	IDS_STR_OrderDefaultQty,						//"93", "合约快速下单默认量！"
	IDS_STR_FastOrderBuySellPriceInva,				//"94", "快速委托买卖价差越界！"
	IDS_STR_IceMaxTip,								//"95", "冰山单最大数量不能大于委托数量！ "
	IDS_STR_IceMinTip,								//"96", "冰山单最小数量不能大于最大数量！ "
	IDS_STR_ConfiSend,								//"97", "确定发出此笔定单吗?"
	IDS_STR_TirgCondiEmpty,							//"98", "触发条件不能为空"
	IDS_STR_OrderQtyInvalid,						//"99", "下单次数输入非法，输入应该大于0"
	IDS_STR_PriceIncreInvalid,						//"100", "价格增量输入有误"
	IDS_STRING_IceMaxInvalid,						//"101", "最大数量输入有误"
	IDS_STRING_IceMinInvalid,						//"102", "最小数量输入有误"
	IDS_STRING_TirgModeKeyTip,						//"103", "最新价:%c 买价:%c 卖价:%c"
	IDS_STRING_UpDownLimit,							//"104", "跌停价:%s   涨停价:%s"
	IDS_STR_POSTEST,								//"105", "持 888"
	IDS_STR_Font_MicYaHei,							//"106", "微软雅黑"
	IDS_STR_SwapInvalid,							//"107", "互换单合约无效！ "
	IDS_STR_OptionInvalid,							//"108", "无效的期权合约！ "
	IDS_STR_OrderConfir,							//"109", "下单确认"
	IDS_BatchOrder,									//"110", "批量"
	IDS_BatchCount,									//"111", "批次"
	IDS_PayOrder,									//"112", "埋单"
	IDS_ConditionEx,								//"113", "条件&T"
	IDS_CoverOpenEx,								//"114", "平开&L"
	IDS_FastOrderEx,								//"115", "快速&K"
	IDS_BatchCountEx,								//"116", "批次&B"
	IDS_InValidOrderCounts,							//"117", "无效的下单次数"
	IDS_InValidQtyInc,								//"18", "无效的数量增量"
	IDS_InValidPriceInc,							//"119", "无效的价格增量"
	IDS_STR_TirgModeEmpty,							//"120", "触发方式不能为空"
	IDS_ConfirmPayAuto,								//"121", "确定 埋单 自动单(内盘)"
	IDS_KeyboardStyle,								//"122", "横向下单按钮风格"
	IDS_OrderQtyInvalid,							//"123", "委托数量不能小于成交量"
	IDS_STR_OrderConfAsk,							//"124", "，确定要下入这边定单吗？"
	IDS_STR_MaxOrderQtyTip,							//"125", "委托数量%u超出最大下单量%u，您确定要下入这边定单吗？"
	IDS_STR_MultiQtyKey,							//"126", "快捷键"
	IDS_STR_MultiQty,								//"127", "数量倍率"
	IDS_STR_InvalidMultiQty,						//"128", "数量倍率不能为0 ！"
	IDS_STR_ExsitMultiQty,							//"129", "快捷键[%ld]已存在，默认量被修改！"
	IDS_STR_RepMultiKey,							//"130", "与[%s]键重复！"
	IDS_STR_LongPosition,							//"131", "多头"
	IDS_STR_ShortPosition,							//"132", "空头"
	IDS_STR_Buy,									//"133", "买入(B)"
	IDS_STR_SELL,									//"134", "卖出(S)"
	IDS_STR_InvalidUserNo,							//"135", "资金账户为空"
	IDS_STR_InvalidGroupNo,							//"136", "组号为空 "
	IDS_STR_NoLoginUser,							//"137", "账户【%s】未登录"
	IDS_STR_IllUserNo,								//"138", "资金账户非法 !"
	IDS_STR_IllGroupNo,								//"139", "组号非法 !"
	IDS_STR_InvalidDirect,							//"140", "买卖方向输入无效"
	IDS_STR_InvalidPrice,							//"141", "价格输入为空"
	IDS_STR_InvalidMolecularPrice,					//"142", "输入价格的分子不能大于分母"
	IDS_STR_NoMiniChangePrice,						//"143", "输入价格不是最小变动价的倍数"
	IDS_STR_InvalidOrderQty,						//"144", "委托数量不能为空或者为0"
	IDS_STR_InvalidMulti,							//"145", "倍率跳动点数不能为0 ！"
	IDS_OK,											//"146", "确定"
	IDS_CANCEL,										//"147", "取消"
	IDS_STR_Last,									//"148", "最新价"
	IDS_STR_SettleT,								//"149", "今结算"
	IDS_STR_SettleY,								//"150", "昨结算"
	IDS_STRING_CheckPrice,							//"151", "该笔定单%s的委托价格【%s】与%s【%s】价差范围超出设定值"
	IDS_STR_Warn,									//"152", "警告"
	IDS_STR_Forbid,									//"153", "禁止下单"
	IDS_STR_Follow,									//"154", "跟盘"
	IDS_STR_OrderType_Show,							//"155", "限价:%c 市价:%c 限价止损:%c 止损:%c"
	IDS_STR_OrderType_Show2,						//"156", "限价:%c 市价:%c 限价止损:%c 止损:%c 冰山:%c 影子:%c"
	IDS_DefaultFocus,								//"157", "委托窗口默认焦点在左起第"
	IDS_Item,										//"158", "项"
	IDS_HightDirect,								//"159", "买卖方向高亮显示"
	IDS_UpDownEqualTab,								//"160", "上下键等于Tab键"
	IDS_LeftRightEqualTab,							//"161", "左右键等于Tab键"
	IDS_PriceSyncQuote,								//"162", "委托价格行情同步刷新"
	IDS_OnlyBuySell,								//"163", "买入 卖出"
	IDS_FontSize,									//"164", "横向下单字体大小"
	IDS_FontSmall,									//"165", "小字体"
	IDS_FontMid,									//"166", "中字体"
	IDS_FontBig,									//"167", "大字体"
	IDS_ShowOrderValidType,							//"168", "显示定单类型,有效类型(内盘)"
	IDS_OnlyOkButton,								//"169", "确定"
	IDS_Seq_Default,								//"170", "默认"
	IDS_Seq_Up,										//"171", "上移"
	IDS_Seq_Down,									//"172", "下移"
	IDS_Seq_OK,										//"173", "确定"
	IDS_Seq_Close,									//"174", "关闭"
	IDS_Market_Zero,								//"175", "价格0下入市价单"
	IDS_FollowPrice,								//"176", "跟盘价格"
	IDS_OrderFollowSwitch,							//"177", "委托跟盘切换"
	IDS_SeqSet,										//"178", "下单项顺序设置"
	IDS_BatchModify,								//"179", "批量修改"
	IDS_Condition,									//"180", "条件"
	IDS_CoverOpen,									//"181", "平开"
	IDS_FastOrder,									//"182", "快速"
	IDS_LockTip,									//"183", "账号锁定状态无法响应改单操作"
	IDS_PosTotal,									//"184", "tab=Position Total"
	IDS_PosDetails,									//"185", "tab=Position Details"
	IDS_CommissionInfo,								//"186", "tab=Commission Info"
	IDS_InnerSupport,								//"187", "策略单目前仅内盘支持"
	IDS_STR_TirgCon_Greater,						//"188", ">="
	IDS_STR_TirgCon_Lesser,							//"189", "<="
	IDS_SameHotKey,									//"190", "快捷键冲突"
	IDS_Setting,									//"191", "横向设置"
	IDS_FastOperator,								//"192", "横向快捷键"
	IDS_Futrue,										//"193", "期货"
	IDS_Option,										//"194", "期权"
	IDS_Spread,										//"195", "套利"
	IDS_InvalidOrderPrice,							//"196", "无效的委托价格"
	IDS_InvalidTrigPrice,							//"197", "无效的触发价格"
	IDS_StartHEnterSend,							//"198", "横向开启Enter键确认发单功能"
	IDS_Good,										//"199", "现货"
	IDS_Defer,										//"200", "延期"
	IDS_CHECK_PK,									//"201","平开&L"
	IDS_CHECK_KS,									//"202","快速&K"
	IDS_CHECK_TJ,									//"203","条件&T"
	IDS_Check_BatchOrder,							//"204","批量&U"
	IDS_Check_BatchCount,							//"205","批次&B"
	IDS_CHECK_T1,									//"206","港T+1"
	IDS_CHECK_ForCover,								//"207", "强平"
	IDS_STATIC_UserNo,								//"208","资金账号"
	IDS_Static_LoginNo,								//"209","登陆名字"
	IDS_STATIC_Code,								//"210","合约代码:"
	IDS_STATIC_Direct,								//"211", "买/卖"
	IDS_STATIC_Offset,								//"212","开/平"
	IDS_STATIC_OrderPrice,							//"213","委托价格"
	IDS_STATIC_OrderQty,							//"214","委托数量"
	IDS_STATIC_QtyMulti,							//"215","数量倍率"
	IDS_STATIC_TrigMode,							//"216","触发方式"
	IDS_STATIC_TrigCondi,							//"217","条件"
	IDS_STATIC_TrigPrice,							//"218","触发价格"
	IDS_STATIC_OrderType,							//"219", "定单类型"
	IDS_STATIC_ValidType,							//"220", "有效类型"
	IDS_STATIC_ValidData,							//"221","有效日期"
	IDS_STATIC_QtyCount,							//"222", "下单次数"
	IDS_STATIC_IncreQty,							//"223", "数量增量"
	IDS_STATIC_IncrePrice,							//"224","价格增量"
	IDS_BUTTON_DoOrder,								//"225","确定(&Y)"
	IDS_BUTTON_PayOrder,							//"226","埋单(&P)"
	IDS_Btn_Seq,									//"227","顺序"
	IDS_BUTTON_AutoOrder,							//"228","自动单"
	IDS_StaticBuyPos,								//"229","多头：8888"
	IDS_StaticSellPos,								//"230","空头：8888"
	IDS_STATIC_MaxQty,								//"231","最大量"
	IDS_STATIC_MinQty,								//"232","最小量"
	IDS_CHECK_AutoPrice,							//"233","跟盘"
	IDS_Btn_Buy,									//"234","买入(B)"
	IDS_Btn_Sell,									//"235","卖出(S)"
	IDS_DefaultPrice,								// 236  默认价格
	IDS_AddFlag,									// 237  加号
	IDS_Tick,										// 238  点
	IDS_SpreadM,				//239"跨期"
	IDS_Straddle,				//240跨式
	IDS_Strangle,				//241宽跨式

	IDS_Use_Assistant = 251,			//"251","开启追单助手"
	IDS_Shortcut_Key,			//"252","快捷键"
	IDS_Add,					//"253","添加"
	IDS_Delete,					//"254","删除"
	IDS_Chase_Open_Close,		//"255","追开平"
	IDS_Chase_Open,				//"256","只追开仓"
	IDS_Chase_Close,			//"257","只追平仓"
	IDS_Contract_Shortcut_Key,			//"258","合约快捷键"

};