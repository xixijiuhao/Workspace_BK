/*
���빫ʽ�������ִ�Сд
ע�������������͸�������ʱ����Ҫ�����Ų��ַŵ�������磺2*-5��ҪдΪ2*(-5)

֧�ֵ����������
LAST		���¼�
BID			���
ASK			���
AVG			����
PRICE		ί�м�
LIMITUP		��ͣ��
LIMITDOWN	��ͣ��
HIGH		��߼�
LOW			��ͼ�
PREVSETTLE	ǰ����
OPEN		���̼�
BIDLOT		����
ASKLOT		����
PERIOD		��ǰ������

֧�ֵ���ѧ������
PI			Բ����
SQRT		ƽ����
ABS			����ֵ
SIN			���Һ���
COS			���Һ���
TAN			���к���
ARCSIN		�����Һ���
ARCCOS		�����Һ���
ARCTAN		�����к���
LN			log
LOG10		10�Ķ���
ROUND		��������
TRUNC		ȡ��

֧�ֵĲ������ż�������˳��
�߼��������
|��||��or		��
&��&&��and		��
�Ƚ��������
=��==��<>��!=	���ڡ����ڡ������ڡ�������
>��<��>=��<=	���ڡ�С�ڡ����ڵ��ڡ�С�ڵ���
��ѧ�������
+��-			�ӡ���
*��/��%			�ˡ�������ģ
^				�˷�
�߼��������
!��not			�ǡ���
��ѧ�������
(��)			�����š�������
*/

#pragma once

#include "afxwin.h"
#include "afxcmn.h"
#include "../../../StkData.h"

//������
typedef struct Tag_ExpResult
{
	double	nData;		//����ֵ
	bool	bData;		//����ֵ
	bool	IsNum;		//����Ƿ�Ϊ����ֵ��true������ֵ��false������ֵ
public:
	Tag_ExpResult()	{	memset(this, 0, sizeof(Tag_ExpResult));	};
}	TExpResult;

//Tag_OperParam.pType��ȡֵ��Χ
#define ptNone	0
#define ptNum	1		//����
#define ptFunc	2		//����
#define ptOper	3		//������
typedef struct Tag_OperParam
{
	char	Value[16];	//����
	byte	pType;		//��������
	int		OperLevel;	//����������ȼ�
	int		Index;		//����������������
}	TOperParam;



class CExpression
{
public:
	bool Execute(CString Formula, TExpResult & Result);							//���ʽ��ֵ���ɷ�����ֵ��Booleanֵ
	bool Execute(CString Formula);												//���ʽ��ֵ������Booleanֵ����������Booleanֵ�򷵻�false
	bool CheckValid(CString Formula);											//���ʽ��Ч�Լ��
	CString GetErrors();
	CString GetWordsInfo();														//��ȡ���������������ϸ��Ϣ
	~CExpression();
private:
	CString m_sErrors;					//������Ϣ
	CArray<TOperParam *> m_Words;		//�����������
	int m_iWordCount;					//��Ч�����������Ŀ
	int m_iReCount;						//��ǰ���ʽ������������
	int m_ErrorBegin, m_ErrorEnd;		//�жϱ��ʽ�������ظ����ֵ�����(���ʽ����ʼλ��)

	int GetOperandLevel(int OperIndex);	//��������ȼ�
	int GetFunctionIndex(CString Word);	//��������
	int GetOperatorIndex(CString Word);	//���������

	void Init();						//��ʼ��
	bool ExpressionCheck(int BeginInd, int EndInd);								//���ݱ��ʽ�������ظ����ִ������б��ʽ��Ч��У��

	TOperParam * AllocaWord();			//����һ��ָ�����������ָ�룬ֻ�������ں���AddWord��ʹ��
	void AddWord(TOperParam * pData);	//����һ���������������m_Words֮��
	bool AddWord(CString Word, byte pType=ptNone, int Index=-1);//����һ���������������m_Words֮��
	bool GetWords(CString Formula);		//�ִʣ��Ի�ȡ���ʽ�е������������
	TOperParam * GetWord(int Index);	//����m_Words��ָ�������Ĳ�������
	void ClearWords(bool Virtual=true);	//�������������壬Virtual=true��ʾ������գ�ֻ�ǽ�m_iWordCount��0���������m_Words���ڴ�ռ��

	TExpResult GetValue(int & BeginInd, int & EndInd);							//���ʽ��ֵ
	double GetFuncValue(int FuncIndex, int & BeginInd, int & EndInd);			//������ֵ������ȡ��ǰ����ĺ���
protected:
	void AddError(CString Value, int BeginIndex=-1, int EndIndex=-1);			//���ݴ����ı��ͳ���λ�����ӳ�����Ϣ
	virtual double GetOtherFuncValue(int FuncIndex, int & BeginInd, int & EndInd)=0;//��չ����
	TExpResult GetOperand(TOperParam * pPrevOper, int & BeginInd, int & EndInd, bool AIsNum=true);//��ȡͬһ���㼶�����һ��������
};

/******************************************** CExpressionEx *********************************************/
class CExpressionEx:public CExpression
{
public:
	bool Execute(CString Formula, TExpResult & Result, double CurrOderPrice=1, CStk * pQuote=NULL);	//���ʽ��ֵ���ɷ�����ֵ��Booleanֵ
	bool Execute(CString Formula, double CurrOderPrice=1, CStk * pQuote=NULL);						//�������ʽ��ֵ�����������ʽ��ֱ�ӷ���false
	bool CheckValid(CString Formula, double CurrOderPrice=1, CStk * pQuote=NULL);					//���ʽ��Ч�Լ��
private:
	CStk * m_pQuote;
	double m_CurrOderPrice;																			//��ǰ������ί�м�
	int GetCurrPeriod(int DataType);
protected:
	virtual double GetOtherFuncValue(int FuncIndex, int & BeginInd, int & EndInd);					//������չ(���麯��)
};