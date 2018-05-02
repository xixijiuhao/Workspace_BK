#pragma once

#define PLUG_LOGGINGAPI_NAME	 	L"LoggingApi"
#define PLUG_LOGGINGAPI_VERSION 	L"9.3.1.3"

//��־������Щ����֮��������߼���������ʹ�ã��໥֮��û�����ȼ��Ĺ�ϵ 
enum TLogLevel
{
	llDebug		= 0,	//���ԣ���Ҫ���ڿ��������У����Ե��Գ��򣬸������������鷢������ȡ���ü���
	llInfo		= 1,	//��Ϣ�������������Ϣ�����罻���������ۣ�ֹ��ֹӯ�������ۣ�ϵͳ�����޸����۵�
	llWarn		= 2,	//���棬����Ǳ�ڴ��󣬵���Ӱ��ϵͳ������������������յ��ظ��ɽ���ĳ��Ʒ�����������������ڵ�
	llError		= 3,	//���󣬵��Բ�Ӱ��ϵͳ�������������
	llFatal		= 4		//��������Ӱ�쵽������������У����ܵ���ҵ����һ�ϵͳ���������
};

//��־�������ͣ������Զ���256���������ͣ�ϵͳռ����ǰ�������ͣ������Ŀ�����ldtString֮���Լ�����
typedef unsigned char TLogType;
#define ltNone			0
#define ltString		1

typedef const wchar_t *		TFileSignType;
typedef const unsigned int	TFileIdType;
typedef const void *		TLogDataType;
typedef const unsigned int  TLogDataSizeType;
typedef const bool			TCodeType;
typedef const bool			TClearModeType;
typedef const bool			TSyncModeType;

class ILoggingApi
{
public:		
	//����־�ļ�������ļ�����������Զ���������
	//���������
	//	sign	:��־�ļ���ʶ��,��Ϊ�������ļ�����·������չ�����ļ���, �����ʶ���а�����б��,��ֻȡ�ұߵ�һ����б��֮��Ĳ�����Ϊ��Ч���ļ���ʶ��,���������չ����������Ϊ���Ǳ�ʶ����һ����
	//	unicode	:�ַ��������Ƿ�Ϊunicode��ʽ,���unicodeΪtrue��������ļ�ͷ����16λunicode�ļ���־
	//	clear	:����ļ�����,�Ƿ�����ļ�ԭ������,������������ļ�β����������
	//	sync	:�Ƿ���ͬ����ʽд����־�ļ�
	//  ����ֵ��
	//	������־�ļ�ID�����������ļ������append_data�ĵ�һ��������Ҫ�������ID���Ա㽫��־����д�뵽��Ӧ���ļ�������0��ʾ�ļ���ʧ��
	virtual TFileIdType __cdecl CreateLogging(TFileSignType sign, TCodeType unicode,  TClearModeType clear, TSyncModeType sync) = 0;

	//��¼������־����TLogDataType��ͬ�����������ͣ��������Ͳ�ͬ��������ʽ��ͬ
	//���������
	//	id		:��־�ļ�ID, ��־���ݽ���д���ID����Ӧ���ļ������ID�ɺ���open_log_file����
	//	pdata	:��־���ݣ������Ƕ����ƻ��ַ���
	//	size	:��־���ݳ���
	//	level	:��ǰд�����ݵ���־�������open_log_file�Ĳ���level�в������ü�����д������ݽ��ᱻϵͳ���ԣ�����д�뵽��־�ļ���ȥ
	//	type	:�������ͣ�ֻ��open_log_file�Ĳ���prefix�а���lpmDataTypeʱ����Ч��
	//			���һ���ļ���Ҫд����ֲ�ͬ���ȵ�������ʱ����ʹ�����������Ϊ���ļ��е�ÿ����������ָ��һ��Ψһ��data_typeֵ, ����data_typeֵ�Ϳ�������־�鿴�����н�����
	//			���һ���ļ���ֻ����ͬһ���͵�������ʱdata_type���Բ���
	virtual void __cdecl OutputLog(TFileIdType id, const  TLogType type, const TLogLevel level, TLogDataType pdata, TLogDataSizeType size) = 0;
};