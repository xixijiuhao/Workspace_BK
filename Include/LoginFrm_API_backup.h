#ifndef _LOGIN_FRM_API_
#define _LOGIN_FRM_API_

const wchar_t PLUG_LOGINFRM_NAME[] = L"LoginFrm";
const wchar_t PLUG_LOGINFRM_VERSION[] = L"1.0.0.1";

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
	//��ȡ������Ϣ
	virtual	 bool   __cdecl		GetErrorInfo(wchar_t* perrorinfo, int nlength) = 0;//����ֵ true: ���ӳɹ� false ��֤ʧ��
};
#pragma pack(pop)

#endif