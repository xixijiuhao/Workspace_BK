/*
输入公式不用区分大小写
注：其他操作符和负号相连时，需要给负号部分放到括号里。如：2*-5需要写为2*(-5)

支持的行情参数：
LAST		最新价
BID			买价
ASK			买价
AVG			均价
PRICE		委托价
LIMITUP		涨停价
LIMITDOWN	跌停价
HIGH		最高价
LOW			最低价
PREVSETTLE	前结算
OPEN		开盘价
BIDLOT		买量
ASKLOT		卖量
PERIOD		当前周期数

支持的数学函数：
PI			圆周率
SQRT		平方根
ABS			绝对值
SIN			正弦函数
COS			余弦函数
TAN			正切函数
ARCSIN		反正弦函数
ARCCOS		反余弦函数
ARCTAN		反正切函数
LN			log
LOG10		10的对数
ROUND		四舍五入
TRUNC		取整

支持的操作符号及其优先顺序：
逻辑运算符：
|、||、or		或
&、&&、and		与
比较运算符：
=、==、<>、!=	等于、等于、不等于、不等于
>、<、>=、<=	大于、小于、大于等于、小于等于
数学运算符：
+、-			加、减
*、/、%			乘、除、求模
^				乘方
逻辑运算符：
!、not			非、非
数学运算符：
(、)			左括号、右括号
*/

#pragma once

#include "afxwin.h"
#include "afxcmn.h"
#include "../../../StkData.h"

//运算结果
typedef struct Tag_ExpResult
{
	double	nData;		//数字值
	bool	bData;		//布尔值
	bool	IsNum;		//结果是否为数字值，true：数字值；false：布尔值
public:
	Tag_ExpResult()	{	memset(this, 0, sizeof(Tag_ExpResult));	};
}	TExpResult;

//Tag_OperParam.pType的取值范围
#define ptNone	0
#define ptNum	1		//数字
#define ptFunc	2		//函数
#define ptOper	3		//操作符
typedef struct Tag_OperParam
{
	char	Value[16];	//内容
	byte	pType;		//参数类型
	int		OperLevel;	//运算符的优先级
	int		Index;		//操作符或函数的索引
}	TOperParam;



class CExpression
{
public:
	bool Execute(CString Formula, TExpResult & Result);							//表达式求值，可返回数值或Boolean值
	bool Execute(CString Formula);												//表达式求值，返回Boolean值，运算结果非Boolean值则返回false
	bool CheckValid(CString Formula);											//表达式有效性检查
	CString GetErrors();
	CString GetWordsInfo();														//获取所有运算参数的详细信息
	~CExpression();
private:
	CString m_sErrors;					//错误信息
	CArray<TOperParam *> m_Words;		//运算参数缓冲
	int m_iWordCount;					//有效运算参数的数目
	int m_iReCount;						//当前表达式连续解析次数
	int m_ErrorBegin, m_ErrorEnd;		//判断表达式连续的重复出现的依据(表达式的起始位置)

	int GetOperandLevel(int OperIndex);	//运算符优先级
	int GetFunctionIndex(CString Word);	//函数索引
	int GetOperatorIndex(CString Word);	//运算符索引

	void Init();						//初始化
	bool ExpressionCheck(int BeginInd, int EndInd);								//根据表达式连续的重复出现次数进行表达式有效性校验

	TOperParam * AllocaWord();			//分配一个指向运算参数的指针，只是用于在函数AddWord中使用
	void AddWord(TOperParam * pData);	//增加一个运算参数到缓冲m_Words之中
	bool AddWord(CString Word, byte pType=ptNone, int Index=-1);//增加一个运算参数到缓冲m_Words之中
	bool GetWords(CString Formula);		//分词，以获取表达式中的所有运算参数
	TOperParam * GetWord(int Index);	//返回m_Words中指定索引的操作参数
	void ClearWords(bool Virtual=true);	//清空运算参数缓冲，Virtual=true表示虚拟清空，只是将m_iWordCount置0，否则清空m_Words的内存占用

	TExpResult GetValue(int & BeginInd, int & EndInd);							//表达式求值
	double GetFuncValue(int FuncIndex, int & BeginInd, int & EndInd);			//函数求值，包含取当前行情的函数
protected:
	void AddError(CString Value, int BeginIndex=-1, int EndIndex=-1);			//根据错误文本和出错位置增加出错信息
	virtual double GetOtherFuncValue(int FuncIndex, int & BeginInd, int & EndInd)=0;//扩展函数
	TExpResult GetOperand(TOperParam * pPrevOper, int & BeginInd, int & EndInd, bool AIsNum=true);//获取同一运算级别的下一个操作数
};

/******************************************** CExpressionEx *********************************************/
class CExpressionEx:public CExpression
{
public:
	bool Execute(CString Formula, TExpResult & Result, double CurrOderPrice=1, CStk * pQuote=NULL);	//表达式求值，可返回数值或Boolean值
	bool Execute(CString Formula, double CurrOderPrice=1, CStk * pQuote=NULL);						//条件表达式求值，非条件表达式则直接返回false
	bool CheckValid(CString Formula, double CurrOderPrice=1, CStk * pQuote=NULL);					//表达式有效性检查
private:
	CStk * m_pQuote;
	double m_CurrOderPrice;																			//当前定单的委托价
	int GetCurrPeriod(int DataType);
protected:
	virtual double GetOtherFuncValue(int FuncIndex, int & BeginInd, int & EndInd);					//函数扩展(行情函数)
};