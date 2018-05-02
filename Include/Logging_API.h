#pragma once

#define PLUG_LOGGINGAPI_NAME	 	L"LoggingApi"
#define PLUG_LOGGINGAPI_VERSION 	L"9.3.1.3"

//日志级别，这些级别之间可以用逻辑或操作组合使用，相互之间没有优先级的关系 
enum TLogLevel
{
	llDebug		= 0,	//调试，主要用在开发过程中，用以调试程序，辅助开发，建议发布版中取掉该级别
	llInfo		= 1,	//信息，程序的运行信息，例如交易数据留痕，止损止盈过程留痕，系统配置修改留痕等
	llWarn		= 2,	//警告，出现潜在错误，但不影响系统继续工作的情况，如收到重复成交，某个品种所属交易所不存在等
	llError		= 3,	//错误，但仍不影响系统继续工作的情况
	llFatal		= 4		//致命错误，影响到程序的正常运行，可能导致业务混乱或系统崩溃的情况
};

//日志数据类型，共可以定义256个数据类型，系统占用了前两个类型，其他的可以在ldtString之后自己定义
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
	//打开日志文件，如果文件不存在则会自动创建并打开
	//输入参数：
	//	sign	:日志文件标识符,即为不包含文件所在路径和扩展名的文件名, 如果标识符中包含反斜杠,则只取右边第一个反斜杠之后的部分作为有效的文件标识符,如果包含扩展名，则仍认为其是标识符的一部分
	//	unicode	:字符串类型是否为unicode格式,如果unicode为true，则会在文件头加入16位unicode文件标志
	//	clear	:如果文件存在,是否清空文件原有内容,如果不清空则从文件尾加入新内容
	//	sync	:是否以同步方式写入日志文件
	//  返回值：
	//	返回日志文件ID，但并不是文件句柄，append_data的第一个参数需要填入这个ID，以便将日志内容写入到相应的文件；返回0表示文件打开失败
	virtual TFileIdType __cdecl CreateLogging(TFileSignType sign, TCodeType unicode,  TClearModeType clear, TSyncModeType sync) = 0;

	//记录数据日志，由TLogDataType共同决定数据类型，数据类型不同，解析方式不同
	//输入参数：
	//	id		:日志文件ID, 日志内容将会写入该ID所对应的文件，这个ID由函数open_log_file返回
	//	pdata	:日志数据，可以是二进制或字符串
	//	size	:日志数据长度
	//	level	:当前写入内容的日志级别，如果open_log_file的参数level中不包含该级别，则写入的内容将会被系统忽略，不会写入到日志文件中去
	//	type	:数据类型，只有open_log_file的参数prefix中包含lpmDataType时才有效，
	//			如果一个文件中要写入多种不同长度的数据体时建议使用这个参数，为该文件中的每种数据类型指定一个唯一的data_type值, 有了data_type值就可以用日志查看器进行解析了
	//			如果一个文件中只存入同一类型的数据体时data_type可以不填
	virtual void __cdecl OutputLog(TFileIdType id, const  TLogType type, const TLogLevel level, TLogDataType pdata, TLogDataSizeType size) = 0;
};