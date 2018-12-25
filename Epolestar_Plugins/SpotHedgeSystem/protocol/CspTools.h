/**
*  @file CspTools.h
*  @brief 公共协议处理库
*  @author fanliangde 2017/9/01
*/

#ifndef _CSP_TOOLS_H_
#define _CSP_TOOLS_H_


#include "CspProtocol.h"

/* RSA密钥类型 */
typedef C8 CspRsaPubKey[200];
typedef C8 CspRsaPriKey[800];

/**
*  @brief 生成RSA密钥对
*
*  @param pubKey RSA公钥
*  @param priKey RSA私钥
*  @return 0: 成功; <0: 失败
*
*  @details 生成RSA密钥对，位数为1024bit，客户端启动时调用
*/
int CspGenRsaKey(CspRsaPubKey pubKey, CspRsaPriKey priKey);

/**
*  @brief 使用RSA公钥加密
*
*  @param output 密文
*  @param outLen 密文长度
*  @param input 明文
*  @param inLen 明文长度
*  @param pubKey RSA公钥
*  @return 0: 成功; <0: 失败
*
*  @details 使用RSA公钥加密，服务端使用公钥加密数据
*/
int CspRsaPubEncrypt(C8 *output, U32 *outLen, C8 *input, U32 inLen, CspRsaPubKey pubKey);

/**
*  @brief 使用RSA私钥解密
*
*  @param output 明文
*  @param outLen 明文长度
*  @param input 密文
*  @param inLen 密文长度
*  @param priKey RSA私钥
*  @return 0: 成功; <0: 失败
*
*  @details 使用RSA私钥解密，客户端使用私钥解密数据
*/
int CspRsaPriDecrypt(C8 *output, U32 *outLen, C8 *input, U32 inLen, CspRsaPriKey priKey);

/* IDEA密钥类型 */
typedef C8 CspIdeaKeyType[512];

/**
*  @brief 使用会话密钥生成idea密钥
*
*  @param key idea密钥
*  @param userKey 会话密钥
*  @param keyLen 会话密钥长度
*
*  @details 使用会话密钥生成idea密钥
*/
void CspSetIdeaKey(CspIdeaKeyType key, const C8 *userKey, U32 keyLen);

/**
*  @brief 使用idea密钥加密
*
*  @param in 明文
*  @param out 密文
*  @param len 明文长度
*  @param key idea密钥
*
*  @details 使用idea密钥加密
*/
void CspIdeaEncrypt(const C8 *in, C8 *out, U32 len, CspIdeaKeyType key);

/**
*  @brief 使用idea密钥解密
*
*  @param in 密文
*  @param out 明文
*  @param len 密文长度
*  @param key idea密钥
*
*  @details 使用idea密钥解密
*/
void CspIdeaDecrypt(const C8 *in, C8 *out, U32 len, CspIdeaKeyType key);

/**
*  @brief 压缩
*
*  @param src 待压缩数据缓存区
*  @param srcLen 待压缩数据长度
*  @param dst 压缩后数据存放缓存区
*  @param dstLen 压缩后数据长度
*  @return 0：成功；-1：失败
*
*  @details 使用LZO算法压缩数据
*/
int CspLzoCompress(const C8 *src, U64 srcLen, C8 *dst, U64 *dstLen);

/**
*  @brief 解压
*
*  @param src 待解压数据缓存区
*  @param srcLen 待解压数据长度
*  @param dst 解压后数据存放缓存区
*  @param dstLen 解压后数据长度
*  @return 0：成功；-1：失败
*
*  @details 使用LZO算法解压数据
*/
int CspLzoDecompress(const C8 *src, U64 srcLen, C8 *dst, U64 *dstLen);

/**
*  @brief 生成指定长度的会话密钥
*
*  @param output 会话密钥
*  @param len 会话密钥长度
*  @return 0: 成功; <0: 失败
*
*  @details 生成指定长度的会话密钥，密钥由可见字符随机生成[0x21, 0x7E]
*/
int GenSessionKey(C8 *output, const U8 len);

/**
*  @brief 生成指定长度的认证串
*
*  @param output 认证串
*  @param len 认证串长度
*  @return 0: 成功; <0: 失败
*
*  @details 生成指定长度的认证串，认证串由可见字符随机生成[0x21, 0x7E]
*/
int GenAuthString(C8 *output, const U8 len);

/**
*  @brief 回调函数
*
*  @param client 客户端id或者指针
*  @param buf 数据
*  @param len 长度
*  @return 0：成功; <0: 失败
*
*  @details 回调函数
*/
typedef int(*CspOnDataFunc)(PTR client, const C8 *buf, const size_t len);

/* 回调函数定义 */
typedef struct CspFrameCallback
{
    CspOnDataFunc func;  /* 回调函数指针*/
    PTR           data;  /* 参数,可能为密钥*/
} CspFrameCallback;

/**
*  @brief 协议解析，将明文数据传入回调函数
*
*  @param client 客户端id或者客户端指针
*  @param buf 从缓存区接收到的数据
*  @param len 数据长度
*  @param callback 回调函数结构指针
*  @return >0: 成功, 已处理数据长度; <=0: 失败
*
*  @details 协议解析，将明文数据传入回调函数
*/
int CspOnFrame(PTR client, const C8 *buf, const size_t len, CspFrameCallback *callback);

/**
*  @brief 填充链路认证请求协议
*
*  @param buf 待填充缓冲区
*  @param bufLen 待填充缓冲区大小
*  @param key RSA公钥
*  @param sysType 子系统类型，对接时使用
*  @return >0: 成功,填充长度; <=0: 失败
*
*  @details 填充链路认证请求协议，客户端调用
*/
int FillAuthReq(C8 *buf, const size_t bufLen, CspRsaPubKey key, CspSubSystemType sysType);

/**
*  @brief 填充链路认证应答协议
*
*  @param buf 待填充缓冲区
*  @param bufLen 待填充缓冲区大小
*  @param key RSA公钥
*  @param sysType 子系统类型，对接时使用
*  @return >0: 成功,填充长度; <=0: 失败
*
*  @details 填充链路认证应答协议，服务器调用
*/
int FillAuthRsp(C8 *buf, const size_t bufLen, CspRsaPubKey key, CspSubSystemType sysType);

/**
*  @brief 解析链路认证应答消息
*
*  @param [in] buf 链路认证应答数据
*  @param [in] len 链路认证应答数据长度
*  @param [in] key RSA私钥,解析会话密钥时使用
*  @return 0: 成功; <0: 失败
*
*  @details 解析链路认证应答消息,解析出会话密钥，客户端调用
*/
int CspOnAuth(C8 *buf, const size_t len, CspRsaPriKey key);


#endif

