#ifndef _LOGIN_FRM_API_
#define _LOGIN_FRM_API_

const wchar_t PLUG_LOGINFRM_NAME[] = L"LoginFrm";
const wchar_t PLUG_LOGINFRM_VERSION[] = L"1.0.0.1";

#pragma pack(push, 1)
struct ProductValue
{
	bool				value; //是否认证
	unsigned int		date; //到期日  YYYYMMDD
};

struct UserAuthent
{
	char			user[21];  //用户名
	ProductValue	value[16]; //16个认证
};

struct UserAuthentEx
{
	unsigned int	serverdate; //登录服务器日期 YYYYMMDD
	UserAuthent     auth;
};

class IAuthent
{
public:
	//获取认证信息
	//char* company   公司名
	//char* psd      密钥
	//返回值：bool 是否取得认证信息
	virtual  bool   __cdecl		GetAuthent(const char* company, const char* psd, UserAuthentEx& cpauthent) = 0;
	//是否经过认证授权
	virtual  bool   __cdecl		IsCertified() = 0;
	//获取错误信息
	virtual	 bool   __cdecl		GetErrorInfo(wchar_t* perrorinfo, int nlength) = 0;//返回值 true: 连接成功 false 认证失败
};
#pragma pack(pop)

#endif