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
	char			group[11]; //��ط���
	bool			bmodify;   //��ط����Ƿ����޸�Ȩ��
};

struct UserAuthentEx
{
	unsigned int	serverdate; //��¼���������� YYYYMMDD
	UserAuthent     auth;
};

struct ConfigGroupReq
{
	char           UserNo[51];
	char           GroupNo[21];
	char           Key[51];
	int			   KeyIndex;

	ConfigGroupReq()
	{
		memset(this, 0, sizeof(ConfigGroupReq));
	}
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

	ConfigGroupRsp()
	{
		memset(this, 0, sizeof(ConfigGroupRsp));
	}
};
typedef ConfigGroupRsp			ConfigGroupAdd;
typedef ConfigGroupReq			ConfigGroupDel;

struct UserPluginReq
{
	char           UserNo[51];
};

struct UserPluginRsp
{
	char           UserNo[51];
	char           PluginNo[21];
	char           GroupNo[21];
	char           BeginDate[11];
	char           EndDate[11];
	char           OperateRight;
};
typedef void(__cdecl *QryConfigFinish)(int ncode);//0:�ɹ���������ʧ��
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

	//���ӷ���
	virtual bool	__cdecl		AddGroupConfigInfo(const ConfigGroupAdd strsp)=0;
	//�޸ķ���
	virtual bool	__cdecl		ModifyGroupConfigInfo(const ConfigGroupRsp strsp) = 0;
	//ɾ������
	virtual bool	__cdecl		DelGroupConfigInfo(const ConfigGroupDel strsp) = 0;
	//��ȡ����
	virtual bool	__cdecl		GetGroupConfigInfo(__inout int& ncount, __out ConfigGroupRsp* pdata = nullptr) = 0;
	//��ȡ�û���Ϣ
	virtual bool	__cdecl		GetPlugUser(__inout int& ncount, __out UserPluginRsp* pdata = nullptr) = 0;
	//���ò�ѯ�ص�����
	virtual void	__cdecl		AddNoticeProc(QryConfigFinish pproc) =0;
};
#pragma pack(pop)

#endif