#ifndef _RC_CONTROL_API_
#define _RC_CONTROL_API_
//#include "QuoteApi_API.h"


const wchar_t PLUG_RISKCONTRL_NAME[] = L"RiskControlApi";
const wchar_t PLUG_RISKCONTRL_VERSION[] = L"1.0.0.1";


const char RC_NOTICE_SENDER[] ="rc_notice_sender"; //发送者
const char RC_RULE_1[]="CONSECUTIVE_LOSS";
const char RC_RULE_2[]="S_CONSECUTIVE_LOSS";
const char RC_RULE_3[]="MAX_LOSS";
const char RC_RULE_4[]="MAX_SINGLE_LOSS";
const char RC_RULE_5[]="MAX_DRAWDOWN";

const char RC_RULE_PRE_CAUTION[]="Approaching_MAX_Loss_Thresh"; //快到预警了


#define RC_MAX_ITEM   5

#define PRE_CAUTION_PERCENT 0.8;
#define RC_ACTION_1_2_LEVEL_DOUBLE 0.5;
#define RC_ACTION_1_4_LEVEL_DOUBLE 0.25;
#define RC_ACTION_1_8_LEVEL_DOUBLE 0.125;
#define RC_ACTION_1_16_LEVEL_DOUBLE 0.0625;


enum RC_ACTION
{
	RC_ACTION_NONE = 0,  
	RC_ACTION_STOP_CHARPTER,	
	RC_ACTION_STOP_HALF_DAY,	
	RC_ACTION_STOP_WHOLE_DAY,	
	RC_ACTION_STOP_ANY_MIN,

};

enum RC_RULE
{
	RC_RULE_NONE = 0,  
	RC_RULE_CONSECUTIVE_LOSS,	
	RC_RULE_S_CONSECUTIVE_LOSS,	
	RC_RULE_MAX_LOSS,	
	RC_RULE_MAX_SINGLE_LOSS,
	RC_RULE_MAX_DRAWDOWN,
};


struct Rc_ui_input_para
{
	bool Choose[RC_MAX_ITEM];
	RC_ACTION _Rc_action[RC_MAX_ITEM];
	int time_m[RC_MAX_ITEM];
	int consecutive_loss_count;
	int s_consecutive_loss_count;
	double max_loss_percent;
	double max_single_loss_percent;
	double max_drawdown;
};


class RiskControlAPI
{
public:
	virtual int Rc_start(Rc_ui_input_para*rc_ui_input)=0;
	virtual int Rc_stop()=0;

};

#endif
