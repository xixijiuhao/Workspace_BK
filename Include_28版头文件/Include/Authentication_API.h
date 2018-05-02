#ifndef _AUTHENTICATION_H_
#define _AUTHENTICATION_H_

//��֤�ӿ�
const wchar_t PLUG_AUTHENTICATIONAPI_NAME[] = L"Authentication";
const wchar_t PLUG_AUTHENTICATIONAPI_VERSION[] = L"9.3.0.1";
#pragma pack(push, 1)
struct ProductValue
{
	bool				value; //�Ƿ���֤
	unsigned int		date; //������  YYYYMMDD
};

struct UserAuthent
{
	char			user[21];  //�û���
	ProductValue	value[16]; //16����֤
};

struct UserAuthentEx
{
	unsigned int	serverdate; //��¼���������� YYYYMMDD
	UserAuthent     auth;
};

class IAuthent
{
public:
	//��ȡ��֤��Ϣ
	//char* company   ��˾��
	//char* psd      ��Կ
	//����ֵ��bool �Ƿ�ȡ����֤��Ϣ
	virtual  bool   __cdecl		GetAuthent(const char* company, const char* psd, UserAuthentEx& cpauthent) = 0;
	//�Ƿ񾭹���֤��Ȩ
	virtual  bool   __cdecl		IsCertified() = 0;
};
#pragma pack(pop)
#endif