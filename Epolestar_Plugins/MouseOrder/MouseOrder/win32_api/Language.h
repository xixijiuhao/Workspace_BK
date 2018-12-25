enum 
{
	IDS_OrderPrice,				//"委托价"
	IDS_LIMIT_PRICE,			//"限 价"
	IDS_MARKET_PRICE,			//"市 价"
	IDS_LIMIT_STOP_PRICE,		//"限价止损"
	IDS_STOP_PRICE,				//"止 损"
	IDS_ASK_PRICE,				//"询 价"
	IDS_REPLY_PRICE,			//"应 价"
	IDS_EXECUTE,				//"行 权"
	IDS_ABANDON,				//"弃 权"
	IDS_ICEBERG,				//"冰 山"
	IDS_SHADOW,					//"影 子"
	IDS_SWAP,					//"互 换"
	IDS_IMMEDIATE_ALL,			//"即时全部"
	IDS_IMMEDIATE_PART,			//"即时部分"
	IDS_EFFECTIVE_THEDAY,		//"当日有效"
	IDS_EFFECTIVE_LONGTERM,		//"长期有效"
	IDS_EFFECTIVE_DEADLINE,		//"限期有效"
	IDS_CLOSE,					//"平 仓"
	IDS_CLOSE_DAY,				//"平 今"
	IDS_OPEN,					//"开 仓"
	IDS_BUY,					//"买 入"
	IDS_SELL,					//"卖 出"
	IDS_GENERALORDER,			//"竖向下单"
	IDS_ARBAPP,					//"套利申请"
	IDS_HEDGEAPP,				//"套保申请"
	IDS_SELLTEXT,				//"卖:"
	IDS_BUYTEXT,				//"买:"
	IDS_TIP,					//"提 示"
	IDS_NONEGATIVE,				//"合约不支持负数价格"
	IDS_STRING1070,				//"不是最小变动价的整数倍"
	IDS_STRING1071,				//"无效的互换合约"
	IDS_STRING1072,				//"此订单类型要求期权合约"
	IDS_STRING1073,				//"无效分数价格"
	IDS_STRING1074,				//"无效委托价格"
	IDS_STRING1075,				//"跟 盘"
	IDS_STRING1076,				//"价格不能为空"
	IDS_STRING1077,				//"账 号"
	IDS_STRING1078,				//"合 约"
	IDS_STRING1079,				//"设 置"
	IDS_STRING1080,				//"选择合约"
	IDS_STRING1081,				//"数 量"
	IDS_STRING1082,				//"选择数量"
	IDS_STRING1083,				//"价格类型"
	IDS_STRING1084,				//"有效类型"
	IDS_STRING1085,				//"最大量"
	IDS_STRING1086,				//"选择最大数量"
	IDS_STRING1087,				//"最小量"
	IDS_STRING1088,				//"选择最小数量"
	IDS_STRING1089,				//"价 格"
	IDS_STRING1090,				//"委托价/跟盘价切换"
	IDS_STRING1091,				//"买入价"
	IDS_STRING1092,				//"选择价格"
	IDS_STRING1093,				//"触发价"
	IDS_STRING1094,				//"选择触发价格"
	IDS_STRING1095,				//"卖出价"
	IDS_STRING1096,				//"选择卖出价格"
	IDS_STRING1097,				//"自动开平"
	IDS_STRING1098,				//"强 平"
	IDS_STRING1099,				//"HK T+1"
	IDS_STRING1100,				//"平 今"
	IDS_STRING1101,				//"确 定"
	IDS_STRING1102,				//"修 改"
	IDS_STRING1103,				//"取 消"
	IDS_STRING1104,				//"多"
	IDS_STRING1105,				//"空"
	IDS_STRING1106,				//"资金账号未登录"
	IDS_STRING1107,				//"无效资金账号"
	IDS_STRING1109,				//"无效交易合约"
	IDS_STRING1110,				//"委托数量不能为0"
	IDS_STRING1111,				//"最大数量不能为0"
	IDS_STRING1112,				//"最小数量不能为0"
	IDS_STRING1113,				//"数量不能为0"
	IDS_STRING1114,				//"套 保"
	IDS_STRING1115,				//"全部持仓"
	IDS_STRING1116,				//"多:%d"
	IDS_STRING1117,				//"空:%d"
	IDS_STRING1118,				//"逐笔撤单"
	IDS_STRING1119,				//"平满撤单"
	IDS_STRING1120,				//"一 档"
	IDS_STRING1121,				//"五 档"
	IDS_STRING1122,				//"最新价"
	IDS_STRING1123,				//"今结算"
	IDS_STRING1124,				//"昨结算"
	IDS_STRING1125,				//"该笔应价单的买入价格【%0.2lf】与%s【%0.2lf】价差范围超出设定值"
	IDS_STRING1126,				//"该笔应价单的卖出价格【%0.2lf】与%s【%0.2lf】价差范围超出设定值"
	IDS_STRING1127,				//"该笔定单的委托价格【%0.2lf】与%s【%0.2lf】价差范围超出设定值"
	IDS_STRING1128,				//"委托数量%u超出最大下单量%u，您确定要下入这笔定单吗？"
	IDS_STRING1129,				//"此报单存在自成交风险，是否继续"
	IDS_STRING1130,				//""
	IDS_STRING1131,				//"%d(对冲:%d 建仓:%d)"
	IDS_STRING1132,				//"预冻结:基币 %0.2lf"
	IDS_STRING1133,				//"卖开"
	IDS_STRING1134,				//"卖平"
	IDS_STRING1135,				//"买开"
	IDS_STRING1136,				//"买平"
	IDS_STRING1137,				//"显示买卖行情"
	IDS_STRING1138,				//"默认价格"
	IDS_STRING1139,				//"最新超价"
	IDS_STRING1140,				//"对盘超价"
	IDS_STRING1141,				//"挂单超价"
	IDS_STRING1142,				//"不默认"
	IDS_STRING1143,				//"下单返回委托列表"
	IDS_STRING1144,				//"委托定单类型"
	IDS_STRING1145,				//"投机"
	IDS_STRING1146,				//"平仓自动撤单"
	IDS_STRING1147,				//"平仓撤单方式"
	IDS_STRING1148,				//"双击行情获取买卖量"
	IDS_STRING1149,				//"双击行情"
	IDS_STRING1150,				//"默认开仓"
	IDS_STRING1151,				//"开 平"
	IDS_STRING1152,				//"单击行情或持仓自动填单"
	IDS_STRING1153,				//"双击行情买卖填充方式"
	IDS_STRING1154,				//"对 盘"
	IDS_STRING1155,				//"挂 单"
	IDS_STRING1156,				//"下单弹出确认窗口"
	IDS_STRING1157,				//"开仓时委价与最新价差范围"
	IDS_STRING1158,				//"下单后定单类型设为限价"
	IDS_STRING1159,				//"平仓时委价与最新价差范围"
	IDS_STRING1160,				//"点"
	IDS_STRING1161,				//"下单后有效类型设为当日有效"
	IDS_STRING1162,				//"单笔最大下单量"
	IDS_STRING1163,				//"手"
	IDS_STRING1164,				//"下单后取消强平选项"
	IDS_STRING1165,				//"单笔委托自动拆分当大于"
	IDS_STRING1166,				//"显示持买持卖"
	IDS_STRING1167,				//"持仓量"
	IDS_STRING1168,				//"单笔持仓"
	IDS_STRING1169,				//"全部持仓"
	IDS_STRING1170,				//"平今平昨自适应"
	IDS_STRING1171,				//"平仓数大于可平数时自动拆分为多笔"
	IDS_STRING1172,				//"自成交提示"
	IDS_STRING1173,				//"中金所市价深度"
	IDS_STRING1174,				//"不 限"
	IDS_STRING1175,				//"tab=Position Total"
	IDS_STRING1176,				//"tab=Position Details"
	IDS_STRING1177,				//"tab=Commission Info"
	IDS_STRING1178,				//"本地套利"
	IDS_STRING1179,				//"先后套利"
	IDS_STRING1180,				//"账号同步"
	IDS_BatchModify,			//"批量修改"
	IDS_ExtraInfo,				//"备 注"
	IDS_UserNoInfo,				//"账 号:%s"
	IDS_UserNameInfo,			//"姓 名:%s"
	IDS_PhoneInfo,				//"电 话:%s"
	IDS_IdentifyInfo,			//"身份证号:%s"
	IDS_AddressInfo,			//"地 址:%s"
	IDS_RemarkInfo,				//"备 注:%s"
	IDS_OrderQtyRate,			//"倍 率"
	IDS_OrderQtyRateTip,		//"数量倍率选择"
	IDS_FastOrder,				//"快 速"
	IDS_LocalStop,				//"客户端止损"
	IDS_StopLoss,				//"止 损"
	IDS_StopFloat,				//"浮 动"
	IDS_StopProfit,				//"止 盈"
	IDS_Keep,					//"保 本"
	IDS_StopEditTip,			//"价格为0不启动对应赢损模式"
	IDS_OrderQtyInvalid,		//"委托数量不能小于成交量"
	IDS_LockTip,				//"账号锁定状态无法响应改单操作\n注:单击[账 号]同步交易列表信息"
	IDS_TradeForbid,			//"交易员无法使用该功能"
	IDS_BuyLossTip,				//"买入 止损触发价要求低于委托价,请重新设置"
	IDS_SellLossTip,			//"卖出 止损触发价要求高于委托价,请重新设置"
	IDS_BuyProfitTip,			//"买入 止盈触发价要求高于委托价,请重新设置"
	IDS_SellProfitTip,			//"卖出 止盈触发价要求低于委托价,请重新设置"
	IDS_BuyKeepTip,				//"买入 保本触发价要求高于委托价,请重新设置"
	IDS_SellKeepTip,			//"卖出 保本触发价要求低于委托价,请重新设置"
	IDS_FloatTip,				//"浮动价格要求大于0"
	IDS_InvalidStopPrice,		//"未设置有效的止损止盈价格"
	IDS_invalidMutilQty,		//"数量倍率无效"
	IDS_StopOnlyFuture,			//"客户端止损仅支持品种类型为期货的合约"
	IDS_Condition,				//"条 件"
	IDS_BatchOrder,				//"批 量"
	IDS_Normal,					//"普 通"
	IDS_BuyPrice,				//"买价"
	IDS_SellPrice,				//"卖价"
	IDS_PreOrder,				//"埋 单"
	IDS_AutoOrder,				//"自动单"
	IDS_Advanced,				//"高 级"
	IDS_PopTip,					//"点击对止损信息进行确认!"
	IDS_ConditionError,			//"条件单信息有误"
	IDS_InvalidQuoteCode,		//"无效的行情合约,止损设置失效"
	IDS_CoverConflictStop,		//"平仓单不会生成止损止盈单"
	IDS_CanOpenCover,			//"可开%d     可平%d"
	IDS_CanOpenCoverT,			//"可开%d 可平今%d 可平昨%d"
	IDS_InvalidBatchGroup,		//"无效的批量分组"
	IDS_InvalidTrigMode,		//"无效触发模式"
	IDS_InvalidTrigCon,			//"无效触发条件"
	IDS_GroupNo,				//"组 号"
	IDS_PosTotal,				//"tab=Position Total"
	IDS_PosDetails,				//"tab=Position Details"
	IDS_CommissionInfo,			//"tab=Commission Info"
	IDS_InvalidSupport,			//"此合约不支持该功能"
	IDS_InvalidTrigPrice,		//"无效的触发价格"
	IDS_Futrue,					//"期货"
	IDS_Option,					//"期权"
	IDS_Spread,					//"套利"
	IDS_InvalidLossPrice,		//"无效的止损价格"
	IDS_MinMaxQty,				//"最大量最小量与委托数量关系无效"
	IDS_MaxOverOrder,			//"冰山单最大下单数量不能大于委托数量"
	IDS_MinOverMax,				//"冰山单最小下单数量不能大于最大下单数量"
	IDS_Good,					//"现货"
	IDS_Defer,					//"延期"
	IDS_LockPosition,			//"锁仓"
	IDS_AddLong,				//"加多"
	IDS_AddShort,				//"加空"
	IDS_CloseLong,				//"平多"
	IDS_CloseShort,				//"平空"
	IDS_NoPositionTip,			//"该合约无持仓,是否继续"
	IDS_Strategys,				//"策 略"	
	IDS_SelectCode,				//"选合约:单击合约/双击编辑框"	
	IDS_InvalidProfitPrice,		//无效的止盈价格
	IDS_AssistOrder,			//追价
	IDS_SpreadPairs,			//份数
	IDS_SpreadM,				//"跨期"
	IDS_TipShow,				//[提示]
	IDS_StopTrig,				//该止损单发出后可能会立即触发或下单失败,是否继续？
	IDS_VertOrder,				//竖向
	IDS_LadderOrder,			//点价
	IDS_MarketBuy,				//市买
	IDS_MarketSell,				//市卖
	IDS_LastMid,				//最新
	IDS_LastMidEx,				//最新(居中)
	IDS_Bid1Mid,				//买一
	IDS_Bid1MidEx,				//买一(居中)
	IDS_Ask1Mid,				//卖一
	IDS_Ask1MidEx,				//卖一(居中)
	IDS_Hand,					//手动
	IDS_STP_Loss,				//STP-止损
	IDS_STL_Loss,				//STL-限价止损
	IDS_LMT,					//LMT-限价
	IDS_CancelB,				//撤买
	IDS_CancelS,				//撤卖
	IDS_CancelA,				//撤全部
	IDS_BuyList,				//买
	IDS_SellList,				//卖
	IDS_CancelTip1,				//是否撤销触发价格【%s】对应的挂单:定单号【%d】
	IDS_CancelTip2,				//是否撤销委托价格【%s】对应的挂单:定单号【%d】
	IDS_InValidCodeF,			//"套利合约第一腿无效"
	IDS_InValidCodeS,			//"套利合约第二腿无效"
	IDS_InValidQuote,			//"无法获取有效行情价格"
	IDS_InValidSpreadCode,		//"套利合约无配比参数"
	IDS_SpreadTab,				//tab=spreadinfo
	IDS_SwapDCE,				//"该交易所不支持互换指令"
	IDS_Reject,					//242 拒绝
	IDS_NoUserLogin,			//243 "交易未登录，无法进行下单操作"
	IDS_SHFEMarket,				//244 上期所不支持市价指令
	IDS_SpreadMatchTrig,		//245 对盘价【%s】达到触发价【%s】,委托后会立即触发,是否继续
	IDS_SpreadWorkTrig,			//246 挂单价【%s】达到触发价【%s】,委托后会立即触发,是否继续	
	IDS_ExUp,					//247 委托价%s超出涨停价%s,是否校正为涨停价发单
	IDS_ExDown,					//248 委托价%s超出跌停价%s,是否校正为跌停价发单		
	IDS_MarketUp,				//249 %s不支持市价指令,是否转为涨停价【%s】发单
	IDS_MarketDown,				//250 %s不支持市价指令,是否转为跌停价【%s】发单
	IDS_Pos,					//251 持仓
	IDS_High,					//252 最高价
	IDS_Low,					//253 最低价
	IDS_SprContract,			//254 更多套利合约...
	IDS_FastOrderCfg,			//255快速下单
	IDS_SpreadSet,				//256"97","套利设置","套利設置","Spread set"
	IDS_MatchSpread,			//257"对盘价"
	IDS_WorkSpread,				//258"挂单价"
	IDS_SyncSpread,				//259"联动"
	IDS_BS,						//260"买/卖"
	IDS_Type,					//261"类型"
	IDS_OptionHead,				//262"期 权"
	IDS_Order,					//263"下 单"
	IDS_CodeShort,				//264"合约"
	IDS_PriceShort,				//265"价格"
	IDS_QtyShort,				//266"数量"
	IDS_Bullish,				//267大涨
	IDS_Bearish,				//268大跌
	IDS_NotRise,				//269不涨
	IDS_NotFall,				//270不跌
	IDS_Break,					//271突破	
	IDS_Dull,					//272盘整
	IDS_BreakStrangle,			//273突破（宽）
	IDS_DullStrangle,			//274盘整（宽）
	IDS_DullRiseCall,			//275盘涨Call
	IDS_DullRisePut,			//276盘涨Put
	IDS_DullFallCall,			//277盘跌Call
	IDS_DullFallPut,			//278盘跌Put
	IDS_AutoGroup,				//279自组
	IDS_GroupType,				//280情境
	IDS_Straddle,				//281跨式
	IDS_Strangle,				//282宽跨式
	IDS_QtyNeedSame,			//283交易所支持的跨式期权组合,需两腿数量保持一致
	IDS_ZeroInvalid,			//284"价格不能为零"
	IDS_TraderForbid,			//285"交易员无法使用该功能"
	IDS_AdvancedInner,			//286"高级|内"
	IDS_AdvancedFor,			//287"高级|外"
	IDS_EXECUTE_ABANDON,		//288"行权|弃权"
	IDS_TargetPrice,			//289 标的价格
	IDS_Real,					//290 实值
	IDS_Virtual,				//291 虚值
	IDS_Equal,					//292 平值
	IDS_BackDirect,				//293 反向
	IDS_AddTarget,				//294 增删标的
	IDS_Reset,					//295 重置合约
	IDS_ExchengSupport,			//296 交易所支持
	IDS_BuyApply,				//297 买入申请
	IDS_SellApply,				//298 卖出申请
	IDS_AskPriceNo,				//299 询价号
	IDS_CrossCom,				//300 交易所无此套利合约，请修改后再申请
	IDS_Available,				//301 可行权数
	IDS_CancelAutoExe,			//302 取消自动行权
	IDS_NotExchangeSupport,		//303 不是交易所不支持的合约
	IDS_AvailableZero,			//304 可行权数0
	IDS_BeforeTime,				//305 结束时间早于当前时间，该时间设置无效，是否继续
	IDS_TimeLate,				//306 超过24小时，交易日切换后定单失效，是否继续
	IDS_TimeCondition,			//307 时间条件	
	IDS_ValidTime,				//308 生效时间
	IDS_SpreadSucceed,			//309 套利条件单【%d】:委托成功 
	IDS_SpreadFailed,			//310 套利条件单:委托失败 
	IDS_OpenContinue,			//311 连续开仓
	IDS_CmbFlag,				//312 买卖方向以第二腿为准
	IDS_NoPrice,				//313 市 价 
    IDS_PreFreeze,              //314 保证金
    IDS_UnsupportType,          //315 不支持的定单类型
    IDS_NonParam,               //316 计算参数不存在
    IDS_NonCommodity,           //317 品种不存在
    IDS_NonQuote,               //318 行情不存在
    IDS_InvalidTrade,           //319 無效的買賣
    IDS_Right_Cancel,           //320 右键撤单
    IDS_Cover_Open,             //321 平开 反手
    IDS_Ladder_Style,           //322 点价样式
    IDS_Ladder_Default,         //323 Default
    IDS_Ladder_Light,           //324 Light
    IDS_Ladder_Dark,            //325 Dark
    IDS_Ladder_Light_Yellow,    //326 Light Yellow
    IDS_Ladder_Light_Blue,      //327 Light Blue
    IDS_Ladder_Light_Green,     //328 Light Green
    IDS_Begin_Time,             //329 开始时间
    IDS_End_Time,               //330 结束时间
    IDS_End_Time_Invalid,       //331 结束时间早于开始时间，该时间设置无效，是否继续
    IDS_Auto_Close,             //332 自对冲申请
    IDS_Option_Auto_Close,      //333 行权前自对冲
    IDS_Future_Auto_Close,      //334 履约后自对冲
    IDS_SHFE_Support_Only,      //335 仅上期所支持的指令
    IDS_EXE_AUTO_CLOSE,         //336 行权后自对冲
    IDS_Buy_Enquiry,            //337 买 询价
    IDS_Sell_Enquiry,           //338 卖 询价
    IDS_InValidCodeThird,   	//339 "套利合约第三腿无效"
    IDS_InValidCodeFourth,		//340 "套利合约第四腿无效"
    IDS_Static_Spread_First,    //341 First
    IDS_Static_Spread_Second,   //342 Second
    IDS_Static_Spread_Third,    //343 Third
    IDS_Static_Spread_Fourth,   //344 Fourth
    IDS_SpreadLastTrig,		    //345 最新价【%s】达到触发价【%s】,委托后会立即触发,是否继续
    IDS_SpreadRatio,            //346 (比例:%s)
    IDS_MovePosition,           //347 移仓
    IDS_Cover_No_Position,      //348 移仓中的平仓合约:%s 没有持仓，是否继续？
    IDS_BuyOpenSellCover,       //349 买开卖平
    IDS_SellOpenBuyCover,       //350 卖开买平
    IDS_BuyCoverSellOpen,       //351 买平卖开
    IDS_SellCoverBuyOpen,       //352 卖平买开
	IDS_AccNo2,					//353 账号2
	IDS_AccNo3,					//354 账号3
	IDS_AccNo4,					//355 账号4
	IDS_ParQtyRight,			//356 居右
	IDS_ParQtyBoth,				//357 左买 右卖
	IDS_ParQtyText,				//358 挂单量样式
};								  