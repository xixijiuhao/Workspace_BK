#pragma once

//�ò������IConfigFrame�ӿ�
#define PLUG_CONFIGFRAME_NAME 		L"ConfigFrame"	//�����
#define PLUG_CONFIGFRAME_VERSION	L"1.0.0.1"		//����汾��Ϣ

struct HWND__;
typedef HWND__* HWND;

//���ô��ڵĴ�������������ֵΪ�ô��ھ��
//	sub_title	: ���ô����ϱ�������Ӳ˵�����
//	parent		: ������
typedef HWND (__stdcall * dlg_create_func)(const wchar_t * /*sub_title*/, const HWND /*parent*/);


///////////////////////////////////////////////////////////////////////////////////////////////////////
enum TConfigMainType
{
	cmtNone	= -1, //��Ч
	cmtGeneral	, //����
	cmtQuote	, //����
	cmtTrade	, //����
	cmtProgram	, //����
	cmtAdvance	, //�߼�
	cmtExpand	  //��չ
};

//���ÿ�ܽӿ�
class IConfigFrame
{
public:
	//ע�����ô��ڵ�ָ���ĵ����˵���
	//���������
	//	sub_title	: ���ô������ӵ����˵�����ʾ�ı��⣬ͬһ���˵���sub_title�����ظ�, ͬһ���˵���title��ͬ���ע��Ļ�ʧ��
	//	create_func	: ���ô��ڴ�������
	//	main_type	: �������˵����ͣ����ڴ˷�Χ�ڵ�ֵ���Զ�תΪcmtExpand����
	//	sub_index	: �ӵ����˵�������������0��ʼ����������ѱ�ռ������Զ�Ϊ��������һ
	//	right		: ע���ߵ�Ȩ�ޣ�Ȩ����֤��ͨ�������˵����ͻ��Զ�תΪcmtExpand����
	//����ֵ��
	//	���ز˵�ID���������ֵΪ0����ע��ʧ�ܣ�ע����ʱ����Ҫ�����ID�������
	virtual unsigned int __stdcall reg_config_dlg(const wchar_t * sub_title, const dlg_create_func create_func, TConfigMainType main_type, int sub_index, const char * right = "") = 0;

	//��ʾ���ÿ�ܣ����л���ָ��ҳ��
	//���������
	//	sub_title	: ���ô������ӵ����˵�����ʾ�ı��⣬���sub_titleΪ�����е��ϴ��˳�ʱ�ı�ǩ
	//	main_type	: �������˵����ͣ��������ǩ����ΪcmtNone�����������ӱ�ǩ�в���sub_title
	//	main_type_mask:�������˵�������ʾ����λͼ
	virtual void __stdcall show(const wchar_t * sub_title = L"", const TConfigMainType main_type = cmtNone, const unsigned char main_type_mask = 0xFF) = 0;
	
	//�����ÿ������ʾ�����������Ϣ
	//���������
	//	hint		: Ҫ��ʾ������
	virtual void __stdcall set_hint(const wchar_t * hint) = 0;

	//ע���˵�������Ϊreg_config_dlg�ķ���ֵ
	virtual void __stdcall unreg_config_dlg(unsigned int menu_id) = 0;
};