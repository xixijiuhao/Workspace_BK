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
	char			group[11]; //风控分组
	bool			bmodify;   //风控分组是否有修改权限
};

struct UserAuthentEx
{
	unsigned int	serverdate; //登录服务器日期 YYYYMMDD
	UserAuthent     auth;
};

struct ConfigGroupReq
{
	char           UserNo[51];
	char           GroupNo[21];
	char           Key[51];
	int			   KeyIndex;
};
struct ConfigGroupRsp
{
	char           UserNo[51];
	char           GroupNo[21];
	char           Key[51];
	int			   KeyIndex;
	char           ValueStr[51];
	int			   ValueInt;
	double         ValueDouble;
};
typedef ConfigGroupRsp			ConfigGroupAdd;
typedef ConfigGroupReq			ConfigGroupDel;
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

	//增加分组
	virtual bool	__cdecl		AddGroupConfigInfo(const char* company, const char* psd, const ConfigGroupAdd strsp)=0;
	//修改分组
	virtual bool	__cdecl		ModifyGroupConfigInfo(const char* company, const char* psd, const ConfigGroupRsp strsp) = 0;
	//删除分组
	virtual bool	__cdecl		DelGroupConfigInfo(const char* company, const char* psd, const ConfigGroupDel strsp) = 0;
	//获取分组
	virtual bool	__cdecl		GetGroupConfigInfo(__in int& ncount, __in ConfigGroupRsp* pdata = nullptr) = 0;
};
#pragma pack(pop)

#endif