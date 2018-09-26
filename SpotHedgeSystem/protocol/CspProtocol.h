/************************************************************************
Copyright (c) 2016-2017 Zhengzhou Esunny Information & Technology Co,.Ltd
File name: QuoteProtocolType.h
Author: Fanliangde     Version:1.0          Date:2017/7/24
Description: 极星后台协议,提供基础数据类型定义
1. 结构体定义采用typedef struct AAA{}AAA;方式
2. 常量定义采用static const Type A = xxx;方式
3. 会话头中子系统号0x00作为公共子系统，其他子系统不得占用
4. 本文件中定义公共数据类型及数据结构，各子系统自行定义相关业务数据类型及结构
5. 统一结构和类型的前缀为Csp,常量前缀为CSP_,其他子系统各自定义前缀(3个字符)
6. 内部字符串编码格式统一为UTF-8
7. 协议号以CMD_开头，子系统使用各自中缀，例如CMD_CSP_XXXX

History:
1. 初始创建，2017.07.24 Fanliangde
2. 评审，2017.08.09
************************************************************************/


#ifndef _CSP_PROTOCOL_H_
#define _CSP_PROTOCOL_H_

#pragma pack(push, 1)

//++++++++++++++++++++++++基础数据类型++++++++++++++++++++++++++++
//布尔类型
typedef signed char                B8;

//有符号整数
typedef signed char                I8;
typedef signed short               I16;
typedef signed int                 I32;
typedef signed long long           I64;

//无符号整数
typedef unsigned char              U8;
typedef unsigned short             U16;
typedef unsigned int               U32;
typedef unsigned long long         U64;

//浮点数
typedef float                      F32;
typedef double                     F64;

//字符类型
typedef  char                      C8;
typedef unsigned short             C16;

//指针类型
typedef void*                      PTR;

//字符串类型
typedef C8                         STR10[11];
typedef C8                         STR20[21];
typedef C8                         STR30[31];
typedef C8                         STR40[41];
typedef C8                         STR50[51];
typedef C8                         STR100[101];
typedef C8                         STR200[201];

//变化长度字符串类型 不保存结束0
typedef struct VSTR8
{
    U8  Len;
    C8  Data[1];
} VSTR8;

//长度为2字节
typedef struct VSTR16
{
    U16  Len;
    C8   Data[1];
} VSTR16;

//长度为4字节
typedef struct VSTR32
{
    U32  Len;
    C8   Data[1];
} VSTR32;

//IPV4/IPV6地址类型定义
typedef STR20                      SIPV4;       //192.168.255.255
typedef STR50                      SIPV6;       //FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:255.255.255.255/120

                                                //MAC地址类型定义
typedef STR20                      SMAC;        //00:17:FA:00:1D:B4

                                                //日期类型定义
typedef STR10                      SDATE;       //日期,YYYY-MM-DD
typedef STR10                      STIME;       //时间,HH:MM:SS
typedef STR20                      SDATETIME;   //日期时间，YYYY-MM-DD HH:MM:SS
typedef STR30                      SLDATETIME;  //日期时间(毫秒)，YYYY-MM-DD HH:MM:SS.XXX
typedef STR30                      SLLDATETIME; //日期时间(微秒)，YYYY-MM-DD HH:MM:SS.XXXXXX

typedef U32                        DATE32;       //日期(整型,日期),20170725
typedef U32                        TIME32;       //时间(整型,小时分钟秒),093015
typedef U32                        LTIME32;      //时间(整型,小时分钟秒毫秒),093015123
typedef U64                        DATETIME64;   //日期时间(整型,日期小时分钟秒),20170725093015
typedef U64                        LDATETIME64;  //日期时间(整型,日期小时分钟秒毫秒),20170725093015123
typedef U64                        LLDATETIME64; //日期时间(整型,日期小时分钟秒微秒),20170725093015123000

                                                 //bool值定义
static const B8                    CSP_TRUE = 1;
static const B8                    CSP_FALSE = 0;

//++++++++++++++++++++++++++++协议头定义++++++++++++++++++++++++++++
////////////////////////////QFrameHead类型定义//////////////////////
//数据帧头标志
typedef C8                         CspFrameFlagType;
//数据帧头唯一标识符
static const CspFrameFlagType      CSP_FRAME_FLAG = 'C';

//数据帧数据内容类型
typedef C8                         CspFrameTypeType;
//明文数据帧
#define                            CSP_FRAME_PLAIN               '1'
//RSA公钥加密
#define                            CSP_FRAME_RSA                 '2'
//压缩数据帧
#define                            CSP_FRAME_LZO                 '3'
//加密数据帧
#define                            CSP_FRAME_IDEA                '4'
//压缩加密数据帧(先压缩后加密)
#define                            CSP_FRAME_LZO_IDEA            '5'


//数据帧数据长度类型(不包含数据帧头长度)
typedef U16                        CspFrameSizeType;
//单帧最小数据长度
static const CspFrameSizeType      CSP_FRAME_MIN             = 0;
//单帧最大数据长度
static const CspFrameSizeType      CSP_FRAME_MAX             = 32 * 1024;

/////////////////////////////CspSessionHead定义//////////////////////////

//协议版本号
typedef U16                        CspProtocolVerType;
//统一协议版本号
static const CspProtocolVerType    CSP_PROTOCOL_VER = 4;

//会话编号类型(标识请求与应答一组会话的对应关系)
typedef U32                        CspSessionIdType;

//终端在单一前置的唯一标识
typedef U32                        CspLinkIdType;

//子系统号,用于标识是否为本系统的协议
typedef U8                         CspSubSystemType;

//增加新闻、统计信息、认证
static const CspSubSystemType      CSP_SUBSYSTEM_CSP         = 0x00;    //当前文件定义协议为公共子系统
static const CspSubSystemType      CSP_SUBSYSTEM_REGISTER    = 0x01;    //用户注册平台
static const CspSubSystemType      CSP_SUBSYSTEM_QUOTE       = 0x02;    //行情平台
static const CspSubSystemType      CSP_SUBSYSTEM_TRADE       = 0x03;    //交易平台
static const CspSubSystemType      CSP_SUBSYSTEM_ACCPOINTER  = 0x04;    //接入点
static const CspSubSystemType      CSP_SUBSYSTEM_PKGMGR      = 0x05;    //包管理平台
static const CspSubSystemType      CSP_SUBSYSTEM_USERSTATS   = 0x06;    //用户行为分析系统
static const CspSubSystemType      CSP_SUBSYSTEM_OPTIONMGR   = 0x07;    //操作员管理
static const CspSubSystemType      CSP_SUBSYSTEM_NEWS        = 0x08;    //新闻资讯平台
static const CspSubSystemType      CSP_SUBSYSTEM_MESSAGE     = 0x09;    //消息发布平台

//协议号(每种功能请求具备单独的协议号)
typedef U16                        CspProtocolCodeType;

//语言类型定义
typedef U16                        CspLanguageType;
static const CspLanguageType       CSP_LANG_NONE             = 0x0000;   //系统默认语言
static const CspLanguageType       CSP_LANG_CHS              = 0x0804;   //简体中文
static const CspLanguageType       CSP_LANG_CHT              = 0x0404;   //繁体中文
static const CspLanguageType       CSP_LANG_ENU              = 0x0409;   //英文

//公司号类型定义
typedef      STR10                 CspCompanyNoType;

//包号类型定义
typedef      STR10                 CspPackageNoType;

//报文链标识(标识报文在上下文的关系)
typedef U8                         CspChainType;
static const CspChainType          CSP_CHAIN_END             = '0';      //没有后续报文
static const CspChainType          CSP_CHAIN_NOTEND          = '1';      //还有后续报文

//数据体长度
typedef U16                        CspFieldSizeType;

//数据体个数
typedef U16                        CspFieldCountType;

//错误码类型
typedef U32                        CspErrorCodeType;

static const CspErrorCodeType      CSP_ERROR_SUCCESS         = 0;        //成功

//认证串类型
typedef STR20                      CspAuthCodeType;

//干扰串类型
typedef U32                        CspDisturbType;

//模块名类型
typedef STR10                      CspModuleNameType;

//通讯密钥类型
typedef STR200                     CspRsaCommKeyType;

//通讯密钥长度
static const U8                    CSP_COMM_KEY_LEN          = 20;

//RSA加密密文长度
static const U8                    CSP_RSA_TEXT_LEN          = 128;

//RSA密钥类型
typedef C8                         CspRsaKeyType[301];

//心跳参数
typedef U16                        CspHBStartDelayType;
typedef U16                        CspHBIntervalType;
typedef U8                         CspHBMaxCountType;

//////////////////////////////////协议格式定义/////////////////////////////

//数据帧头结构,4个字节
typedef struct CspFrameHead
{
    CspFrameFlagType               FrameFlag;          //数据帧头标识
    CspFrameTypeType               FrameType;          //数据帧类型
    CspFrameSizeType               FrameSize;          //数据帧长度(0-32K)
} CspFrameHead;

//行情会话数据头结构，58个字节
//2+2+2+11+1+2+4+4+1+2+2+4+21
typedef struct CspSessionHead
{
    CspProtocolVerType             ProtocolVer;        //协议版本号
    CspLanguageType                LangType;           //语言类型
    CspPackageNoType               PackageNo;          //包号
    CspSubSystemType               SubSystem;          //子系统号,解析协议使用，防止协议重复
    CspProtocolCodeType            ProtocolCode;       //消息类型
    CspSessionIdType               SessionId;          //会话号，客户端填写
    CspLinkIdType                  LinkId;             //间接层客户端id
    CspChainType                   Chain;              //报文链标识
    CspFieldSizeType               FieldSize;          //报文长度
    CspFieldCountType              FieldCount;         //报文个数
    CspErrorCodeType               ErrorCode;          //错误码
    CspAuthCodeType                AuthCode;           //认证串(短连接使用,服务器生成的随机串,具有有效时间,用于身份识别)
} CspSessionHead;

//心跳协议,无数据实体
static const CspProtocolCodeType   CMD_CSP_HeartbeatReq = 0x0000;
static const CspProtocolCodeType   CMD_CSP_HeartbeatRsp = 0x0001;

//链路认证协议
static const CspProtocolCodeType   CMD_CSP_AuthReq      = 0x0002;
static const CspProtocolCodeType   CMD_CSP_AuthRsp      = 0x0003;

//链路认证请求报文必须压缩
typedef struct CspAuthReq
{
    CspDisturbType                 Disturb0;
    CspSubSystemType               SubSystemType;  //子系统类型标识,填写本端子系统号，对端认证，认证失败断开连接
    CspDisturbType                 Disturb1;
    CspDisturbType                 Disturb2;
    CspDisturbType                 Disturb3;
    CspRsaKeyType                  RsaPubKey;      //RSA公钥
    CspDisturbType                 Disturb4;
} CspAuthReq;

//链路认证应答报文必须使用RSA公钥加密
typedef struct CspAuthRsp
{
    CspSubSystemType               SubSystemType;  //系统类型标识,服务器返回自身子系统类型，客户端验证是否为指定对接的子系统
    CspRsaCommKeyType              SendKey;        //客户终端以后发送数据加密使用的Key
    CspRsaCommKeyType              RecvKey;        //客户终端以后接收数据加密使用的Key
    CspHBStartDelayType            HBStartDelay;   //心跳启动延时，没有收到数据到开始心跳请求之间的延时(毫秒)
    CspHBIntervalType              HBInterval;     //心跳请求间隔，发起心跳请求的频率(毫秒)
    CspHBMaxCountType              HBMaxCount;     //最大心跳次数，连续无应答的心跳次数达到此则认为链路已断
} CspAuthRsp;

/*******************************JSON数据格式***********************
结构: CspSessionHead + data struct
协议：使用https
说明: json对象1个会话头和多个数据体组成，其中会话头和数据体与C结构一致
示例：补充双结构，数组型，双结构数组型
var jsondata =
{
CspSessionHead : [{
ProtocolVersion:3,
SessionId:1,
LinkId:1,
SubSystemCode:0x01,
ProtocolCode:0x1001,
LangType:0x0804,
PackageNo:"100101",
Chain:'0',
FieldSize:80,
FieldCount:1,
ErrorCode:0
AuthCode : "123456"
}],
CspDataRsp : [{
ModuleType:'I',
UserName:"esunny",
Password:"123456",
LicenseNo:"epolestar v9.3",
ReservedInfo:""
}],
CspDataRsp2 : [{
ModuleType:'I',
UserName:"esunny",
Password:"123456",
LicenseNo:"epolestar v9.3",
ReservedInfo:""
}],
} */

#pragma pack(pop)



#endif