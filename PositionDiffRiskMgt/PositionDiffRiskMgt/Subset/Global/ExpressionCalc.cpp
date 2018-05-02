#include "stdafx.h"
#include "math.h"
#include "ExpressionCalc.h"
#include "Global.h"
#include "load_rc.h"
#include "sbuset_res.h"

//1-2*(4+5)+Cos(10+20)+(3+7)*-(1+4)*2+(2*5)*(2*5)+2*2^2^2+2^2^2*2 % 5+Sin(90/2)*2
//1-2*(4+5) > Cos(10+20)+(3+7)*(-1+4)*2 || !(2*5)*(2*5) > 2*2^2^2+2^2^2*2 % 5+Sin(90/2)*2

//#define EXPRESSION_ERROR	"�޷������ı��ʽ�����ܰ����Ƿ��ַ���ȱ�ٲ�����"
//#define DIV_ZERO_ERROR		"����������Ϊ��"
//#define LOSS_LEFT_PAREN		"ȱ��������"
//#define LOSS_RIGHT_PAREN	"ȱ��������"
//#define LOSS_OPERATOR		"ȱ��ǰ׺�����"
//#define LOSS_OPERAND		"ȱ���������"
//#define UNVAILD_FUNCNAME	"��Ч�ĺ�����"
//#define UNVAILD_TICKDATA	"����K����ʷ���ݴ���"
//#define UNVAILD_EXPRESSION	"��Ч�ı��ʽ"
//#define OPERAND_TYPE_ERROR	"�����������������ǰ����������ƥ��"
//#define OPERATOR_TYPE_ERROR	"���������ǰ��������������Ͳ�ƥ��"
//#define NOTQUOTEDATA		"û����Ӧ����������"
//#define UNVAILD_BOOL_EXP	"����һ���������ʽ���������������ʽ"
//#define UNVAILD_CHAR		"���ʽ�д��ڷǷ��ַ�"

#define MaxOperLevel		100	//������������ȼ�
#define MAX_RECOUNT			1	//��������ظ������������˴�������Ϊ�޷�����

#define Function_Count		26	//������Ŀ	
#define Operator_Count		24	//��������Ŀ

int CExpression::GetFunctionIndex(CString Word)
{
	Word.MakeUpper();
	return CValueIn::IndexOf(Word, Function_Count, 
		"LAST","BID","ASK","AVG","PRICE","LIMITUP","LIMITDOWN","HIGH","LOW","PREVSETTLE","OPEN","BIDLOT","ASKLOT","PERIOD",
		"PI","SQRT","ABS","SIN","COS","TAN","ARCSIN","ARCCOS","ARCTAN","LN","LOG10","ROUND","TRUNC");
}

int CExpression::GetOperatorIndex(CString Word)
{	
	Word.MakeUpper();
	return CValueIn::IndexOf(Word, Operator_Count, 
		"|", "||", "OR", "&", "&&", "AND", "!", "NOT",
		"=", "==", "<>", "!=", ">", "<", ">=", "<=",
		"+", "-", "*", "/", "%", "^", "(", ")");
}
//��������ȼ�
int CExpression::GetOperandLevel(int OperIndex)
{
	switch (OperIndex)
	{
	case 0: 
	case 1: 
	case 2:	
		return 1;//| || or
	case 3: 
	case 4: 
	case 5:	
		return 2;//& && and

	case 8: 
	case 9:	
	case 10:
	case 11:	
		return 3;//= == <> !=
	case 12:					
	case 13:					
	case 14:					
	case 15: 
		return 4;//> < >= <=

	case 16:
	case 17:
		return 5;//+ -
	case 18:
	case 19:
	case 20:
		return 6;//* / %
	case 21:
		return 7;//^

	case 6: 
	case 7:	
		return 8;//! not

	case 22:
	case 23:
	case 255:
		return MaxOperLevel;//���ʽ��ʼ��û�������
	default:
		return 0xEFFFFFFF;	//��������������ַ�
	}
}
//��ȡ���������������ϸ��Ϣ
CString CExpression::GetWordsInfo()
{
	CString Result="Word	Type	Index	OperLevel";
	CString Ind;
	for (int i = 0; i<m_iWordCount; i++)
	{
		Result = Result+char(13)+char(10)+GetWord(i)->Value;
		Ind.Format("%d	%d", GetWord(i)->Index, GetWord(i)->OperLevel);
		if (GetWord(i)->pType==ptFunc)
			Result=Result+"	ptFunc	"+Ind;
		else if (GetWord(i)->pType==ptOper)
			Result=Result+"	ptOper	"+Ind; 
		else if (GetWord(i)->pType==ptNum)
			Result=Result+"	ptNum	"+Ind;
		else
			Result=Result+"	ptNone	"+Ind;
	}
	return Result;
}
void CExpression::Init()
{
	m_sErrors="";
	m_iReCount=0;
}

CExpression::~CExpression()
{
	ClearWords(false);
}
void CExpression::ClearWords(bool Virtual)
{
	m_iWordCount=0;
	if (Virtual)
		return;

	for (int i=0; i<m_Words.GetCount(); i++)
		delete m_Words[i];
	m_Words.RemoveAll();
}
//����һ������������ռ䲻��ʱAdd������ֱ�Ӹ�ֵ��m_iWordCount��ʾ��Ч�����������Ŀ
void CExpression::AddWord(TOperParam * pData)
{
	if (m_Words.GetCount()<=m_iWordCount)
		m_Words.Add(pData);
	m_iWordCount++;
}
//����һ���������ָ�룬�������л���ʱֱ��ȡ�ã�������new���䡣m_iAllocaCount��ʾ���������Ŀ
TOperParam * CExpression::AllocaWord()
{
	TOperParam * Result=NULL;

	if ((m_iWordCount>=0)&&(m_iWordCount<m_Words.GetCount())) 
		Result=m_Words[m_iWordCount];
	if (Result==NULL)
		Result=new  TOperParam();

	memset(Result, 0, sizeof(TOperParam));

	return Result;
}
//��ȡһ��������
TOperParam * CExpression::GetWord(int Index)
{
	if ((Index<0)||(Index>=m_iWordCount)) 
	{
		AddError(LoadRC::load_str(IDS_EXPRESSION_ERROR).c_str());
		return NULL;
	}
	else
		return m_Words[Index];
}
bool CExpression::AddWord(CString Word, byte pType, int Index)
{
	if ((Word=="")||(pType==ptNone))
		return false;
	else if ((pType==ptOper)&&(Index<0))
	{
		Index=GetOperatorIndex(Word);
		if (Index<0)
			return false;
	}
	else if ((pType==ptFunc)&&(Index<0))
	{
		Index=GetFunctionIndex(Word);
		if (Index<0)
		{
			Index=GetOperatorIndex(Word);
			if (Index>=0)
				pType=ptOper;
		}

		if (Index<0)
			return false;
	}
	
	TOperParam * pWord=NULL;

	pWord=AllocaWord();
	if (Word.GetLength()>=sizeof(pWord->Value))
		return false;
	strcpy_s(pWord->Value, sizeof(pWord->Value), Word);
	pWord->pType=pType;
	pWord->Index=Index;
	if (pType==ptOper)
		pWord->OperLevel=GetOperandLevel(Index);

	AddWord(pWord);

	return true;
}
//�ִʣ��ӱ��ʽ�л�ȡ�����������
bool CExpression::GetWords(CString Formula)
{
	Formula=Formula+' ';

	CString Word, Char;
	int CurrOperInd, OldOperInd=-1, OperInd=-1;
	byte CurrType, OldType=ptNone;
	bool IsAdd=false, AddResult;

	ClearWords();
	int i=0;	
	while (i < Formula.GetLength())
	{		
		//�Ǵ�ӡ�ַ�
		if ((Formula[i]<32)||(Formula[i])>126)
		{
			Word.Format("%s, col: %d%c%c%s", LoadRC::load_str(IDS_UNVAILD_CHAR).c_str(), i+1, char(13),char(10), Formula);
			AddError(Word);
			return false;
		}
		Char=Formula[i];
		CurrOperInd=GetOperatorIndex(Char);
		//��ǰ�ַ�����
		if (((OldType!=ptFunc))&&((isdigit(Char[0]) || Char[0]=='.')))
			CurrType=ptNum;
		else if (CurrOperInd>=0)
			CurrType=ptOper;
		else if (Char[0]==' ')
			CurrType=ptNone;
		else
			CurrType=ptFunc;

		//�ж��Ƿ���ӵ���
		if (Word!="")
			switch (OldType)
			{
				case ptNum:													//һ��������
					IsAdd = (CurrType==ptOper)||(CurrType==ptNone);	break;
				case ptFunc:												//�����Ǻ�����and.or.not������ �� �Ƿ�����
					IsAdd = (CurrType==ptOper)||(CurrType==ptNone);	break;
				case ptOper:												//�����ǲ����� �� �Ƿ�����
					IsAdd = CurrType!=ptOper;
					IsAdd = IsAdd || (CurrOperInd==22)||(OldOperInd==23);
					IsAdd = IsAdd || (CurrOperInd==6)||(CurrOperInd==7);
					IsAdd = IsAdd || (((CurrOperInd==17)||(CurrOperInd==17))&&
									  ((OldOperInd>=0)&&(OldOperInd<=15)||(OldOperInd==22)));
					if ((IsAdd)&&(Word.GetLength()==1))						//һ���ǲ�����
						OperInd=OldOperInd;							break;
			}//switch

		//��ӵ���
		if (IsAdd)
		{
			AddResult=AddWord(Word, OldType, OperInd);
			if (! AddResult)
			{
				Word.Format("%s%c%c%s", LoadRC::load_str(IDS_UNVAILD_EXPRESSION).c_str(), char(13),char(10),
					Formula.Left(i-Word.GetLength()) + "[" + Word + "]" + Formula.Mid(i, Formula.GetLength()-1-i));
				AddError(Word);
				return false;
			}
			Word="";
			IsAdd=false;
			OperInd=-1;
		}//IsAdd

		//�ϴ�״̬����
		OldType=CurrType;
		OldOperInd=CurrOperInd;
		if (CurrType!=ptNone)
			Word=Word+Char;

		i++;
	}
	if (m_iWordCount<=0)
	{
		AddError(LoadRC::load_str(IDS_UNVAILD_EXPRESSION).c_str());
		return false;
	}

	return true;
}
//���ʽ��ֵ
bool CExpression::Execute(CString Formula, TExpResult & Result)
{
	Init();
	if (GetWords(Formula))
	{		
		int BIndex=0;
		int EIndex=m_iWordCount-1;
		Result= GetValue(BIndex, EIndex);
	}
	return m_sErrors=="";
}
//���ʽ��ֵ
bool CExpression::Execute(CString Formula)
{
	TExpResult Result;
	Init();
	if (GetWords(Formula))
	{	
		int BIndex=0;
		int EIndex=m_iWordCount-1;
		Result= GetValue(BIndex, EIndex);
		if (Result.IsNum)
			AddError(LoadRC::load_str(IDS_UNVAILD_BOOL_EXP).c_str(), 0, m_iWordCount-1);
	}
	return (m_sErrors=="")&&(!Result.IsNum)&& Result.bData;
}
bool CExpression::CheckValid(CString Formula)
{
	TExpResult Result;
	Init();
	if (GetWords(Formula))
	{	
		int BIndex=0;
		int EIndex=m_iWordCount-1;
		Result= GetValue(BIndex, EIndex);
		if (Result.IsNum)
			AddError(LoadRC::load_str(IDS_UNVAILD_BOOL_EXP).c_str(), 0, m_iWordCount-1);
	}
	return (m_sErrors=="")&&(!Result.IsNum);
}
//��������������б��ʽ��ֵ
TExpResult CExpression::GetValue(int & BeginInd, int & EndInd)
{
	int OldBegin;		 //��һ����������ԭʼ��ʼλ��
	TExpResult Result, Operand;
	TOperParam * pWord;

	pWord=GetWord(BeginInd);
	if (pWord==NULL) 
		return Result;
	Result = GetOperand(NULL, BeginInd, EndInd, (pWord->pType!=ptOper)||(pWord->Index >=12)&&(pWord->Index <22));

	while (BeginInd<=EndInd)
	{
		//��ȡ������
		pWord=GetWord(BeginInd);
		BeginInd++;
		if (pWord==NULL)
			return Result;

		if (pWord->pType!=ptOper)
		{
			AddError(LoadRC::load_str(IDS_LOSS_OPERATOR).c_str(), BeginInd-1, BeginInd-1);
			return Result;
		}
		//�����У��
		if (((pWord->Index >= 12) != Result.IsNum)&&
			((pWord->Index<8)||(pWord->Index>11)))//���������ǰ����������������Ƿ�ƥ�䣬pWord->Index >= 8��Ϊǰ����ֵ�����Ĳ�����
		{
			AddError(LoadRC::load_str(IDS_OPERATOR_TYPE_ERROR).c_str(), BeginInd-1, BeginInd-1);
			return Result;
		};

		OldBegin=BeginInd;
		Operand=GetOperand(pWord, BeginInd, EndInd, Result.IsNum);
		switch (pWord->Index)
		{
			case 0: case 1: case 2:	Result.bData = Result.bData ||  Operand.bData;	break;
			case 3: case 4: case 5:	Result.bData = Result.bData &&  Operand.bData;	break;
			case 6: case 7:			Result.bData =              !   Operand.bData;	break;

			case 8: case 9:			
				if (Result.IsNum)	Result.bData = Result.nData ==   Operand.nData;	
				else				Result.bData = Result.bData ==   Operand.bData;	break;
			case 10:case 11:		
				if (Result.IsNum)	Result.bData = Result.nData !=  Operand.nData;	
				else				Result.bData = Result.bData !=  Operand.bData;	break;
			case 12:				Result.bData = Result.nData >   Operand.nData;	break;
			case 13:				Result.bData = Result.nData <   Operand.nData;	break;
			case 14:				Result.bData = Result.nData >=  Operand.nData;	break;
			case 15:				Result.bData = Result.nData <=  Operand.nData;	break;

			case 16:				Result.nData = Result.nData +   Operand.nData;	break;
			case 17:				Result.nData = Result.nData -   Operand.nData;	break;
			case 18:				Result.nData = Result.nData *   Operand.nData;	break;
			case 19:
				if (Operand.nData != 0)
					Result.nData = Result.nData / Operand.nData;
				else
				{
					AddError(LoadRC::load_str(IDS_DIV_ZERO_ERROR).c_str(), OldBegin, BeginInd-1);
					return Result;
				} 																	break;
			case 20:
				if (CMathFun::Trunc(Operand.nData) != 0)
					Result.nData = CMathFun::Trunc(Result.nData) % CMathFun::Trunc(Operand.nData);
				else
				{
					AddError(LoadRC::load_str(IDS_DIV_ZERO_ERROR).c_str(), OldBegin, BeginInd-1);
					return Result;
				}																	break;
			case 21:				Result.nData = pow(Result.nData, Operand.nData);break;
		}		
		Result.IsNum = pWord->Index >= 16;
	};
	
	return Result;
}
//���ݱ��ʽ�������ظ����ִ������б��ʽ��Ч��У��
bool CExpression::ExpressionCheck(int BeginInd, int EndInd)
{
	if ((m_ErrorBegin==BeginInd)&&(m_ErrorEnd==EndInd))
		m_iReCount++;
	else
	{
		m_ErrorBegin=BeginInd;
		m_ErrorEnd=EndInd;
		m_iReCount=0;
	}
	return m_iReCount > MAX_RECOUNT;
}

//��ȡͬһ���㼶�����һ��������
TExpResult CExpression::GetOperand(TOperParam * pPrevOper, int & BeginInd, int & EndInd, bool IsNum)
{
	TExpResult Result;

	if (ExpressionCheck(BeginInd, EndInd))
	{
		AddError(LoadRC::load_str(IDS_EXPRESSION_ERROR).c_str(), BeginInd, EndInd);
		return Result;
	};

	int PrevOperLevel;			//����Ĳ����������ȼ�
	TOperParam * pWord;
    CString Res;                                                                           
	int BInd, EInd;				//��ȡ���Ĳ���������ֹλ��
	int ParenDeep,				//�������
		ParenRang,				//�������ȼ���������Ŀ
		FuncIndex, FuncPos,		//���������ͺ�����ʼλ��
		BeginPos, EndPos;		//�ӱ��ʽ����ֹλ��
	bool FirstType=((pPrevOper==NULL)||(pPrevOper->Index==22));//(BeginInd==0)&&

	ParenRang= 0;
	ParenDeep= 0;
	BeginPos =BeginInd;
	EndPos   =BeginInd;
	FuncPos  =BeginInd;
	if (pPrevOper==NULL)
		PrevOperLevel=MaxOperLevel;
	else
		PrevOperLevel=pPrevOper->OperLevel;

	pWord=GetWord(BeginInd);
	if (pWord==NULL)
		return Result;
	if (pWord->pType==ptFunc)
		FuncIndex=pWord->Index;
	else
		FuncIndex=-1;

	while (BeginInd <= EndInd)
	{
		pWord=GetWord(BeginInd);
		if (pWord->pType==ptOper)
		{
			if (pWord->Index==22)
			{				
				ParenDeep++;
				if ((ParenDeep==1))
					FuncPos=BeginInd;
			} else if (pWord->Index==23)
			{
				ParenDeep--;
				if (ParenDeep==0)
					ParenRang++;
			}
			else if ((PrevOperLevel >= pWord->OperLevel) && (ParenDeep == 0))	//�Ƿ�һ��ͬ�ȼ��Ĳ�����
				break;
		}
			
		if (ParenDeep<0)
		{
			AddError(LoadRC::load_str(IDS_LOSS_LEFT_PAREN).c_str());
			return Result;
		}

		Res = Res + pWord->Value;
		EndPos=BeginInd;

		BeginInd++;
	}//while
	if (ParenDeep > 0)
	{
		AddError(LoadRC::load_str(IDS_LOSS_RIGHT_PAREN).c_str());
		return Result;
	}

	EndPos=max(EndPos,BeginPos);
	BInd=BeginPos;
	EInd=EndPos;

	//ȥ����β������
	if ((Res != "") && (Res[0] == '(') && (Res[Res.GetLength()-1] == ')')&&(ParenRang==1))
	{
		BeginPos++;
		EndPos--;
		Res.Delete(Res.GetLength()-1, 1);
		Res.Delete(0, 1);
	}

	if (Res == "")
	{
		if ((pWord->Index != 16) && (pWord->Index != 17) && (pWord->Index != 6) && (pWord->Index != 7))
			AddError(LoadRC::load_str(IDS_LOSS_OPERAND).c_str(), BeginPos, EndPos);
		Result.IsNum=IsNum;
		return Result;
	}
	else if ((FuncIndex >= 0) &&                                                                 //����
			 ((Res[Res.GetLength()-1] == ')') && (ParenRang==1) ||                               //��������
			  (BeginPos==EndPos)))                                                               //�޲�����
	{
		Result.nData = GetFuncValue(FuncIndex, FuncPos, EndPos);
		Result.IsNum = true;
	}
	else if ((FuncIndex < 0)&&(BeginPos==EndPos)&&(GetWord(EndPos)->pType==ptNum))               //������
	{
		Result.nData = atof(Res);
		Result.IsNum = true;
	}
	else                                                                                         //���ʽ
		Result=GetValue(BeginPos, EndPos);

	//�����У��
	if ((Result.IsNum != IsNum)&& ! FirstType)
		AddError(LoadRC::load_str(IDS_OPERAND_TYPE_ERROR).c_str(), BInd, EInd);

	return Result;
}

//������ֵ
double CExpression::GetFuncValue(int FuncIndex, int & BeginInd, int & EndInd)
{
	switch (FuncIndex)
	{
		case 14: return CMathFun::PI();	
		case 15: return sqrt (GetOperand(NULL, BeginInd, EndInd).nData);	
		case 16: return abs  (GetOperand(NULL, BeginInd, EndInd).nData);	
		case 17: return sin  (GetOperand(NULL, BeginInd, EndInd).nData);	
		case 18: return cos  (GetOperand(NULL, BeginInd, EndInd).nData);	
		case 19: return tan  (GetOperand(NULL, BeginInd, EndInd).nData);	
		case 20: return asin (GetOperand(NULL, BeginInd, EndInd).nData);	
		case 21: return acos (GetOperand(NULL, BeginInd, EndInd).nData);	
		case 22: return atan (GetOperand(NULL, BeginInd, EndInd).nData);	
		case 23: return log  (GetOperand(NULL, BeginInd, EndInd).nData);	
		case 24: return log10(GetOperand(NULL, BeginInd, EndInd).nData);	
		case 25: return CMathFun::Round(GetOperand(NULL, BeginInd, EndInd).nData);	
		case 26: return CMathFun::Trunc(GetOperand(NULL, BeginInd, EndInd).nData);	
		default: return GetOtherFuncValue(FuncIndex, BeginInd, EndInd);
	}
}

void CExpression::AddError(CString Value, int BeginIndex, int EndIndex)
{
	if (m_sErrors != "")
		return;

	if (BeginIndex+EndIndex>=0)
		for (int i = 0; i<m_iWordCount; i++)
			if ((i==BeginIndex)&&(i==EndIndex))
				m_sErrors = m_sErrors+"["+(GetWord(i)->Value)+"]";
			else if (i==BeginIndex)
				m_sErrors = m_sErrors+"["+GetWord(i)->Value;
			else if (i==EndIndex)
				m_sErrors = m_sErrors+GetWord(i)->Value+"]";
			else
				m_sErrors = m_sErrors+GetWord(i)->Value;

	m_sErrors=Value+char(13)+char(10)+m_sErrors;
}

CString CExpression::GetErrors()
{
	return m_sErrors;
}
/******************************************* CExpressionEx ***********************************************/
bool CExpressionEx::Execute(CString Formula, TExpResult & Result, double CurrOderPrice, CStk * pQuote)
{
	m_pQuote=pQuote;									//���õ�ǰ��Լ����������
	m_CurrOderPrice=CurrOderPrice;						//���õ�ǰ������ί�м�

	return CExpression::Execute(Formula, Result);
}

bool CExpressionEx::Execute(CString Formula, double CurrOderPrice, CStk * pQuote)
{
	m_pQuote=pQuote;									//���õ�ǰ��Լ����������
	m_CurrOderPrice=CurrOderPrice;						//���õ�ǰ������ί�м�

	return CExpression::Execute(Formula);
}
bool CExpressionEx::CheckValid(CString Formula, double CurrOderPrice, CStk * pQuote)
{
	m_pQuote=pQuote;									//���õ�ǰ��Լ����������
	m_CurrOderPrice=CurrOderPrice;						//���õ�ǰ������ί�м�

	return CExpression::CheckValid(Formula);
}
//���麯������ȡ��ǰ��������
double CExpressionEx::GetOtherFuncValue(int FuncIndex, int & BeginInd, int & EndInd)
{
	if (m_pQuote==NULL)
	{
		AddError(LoadRC::load_str(IDS_NOTQUOTEDATA).c_str(), BeginInd-1, BeginInd-1);
		return 0;
	}
	switch (FuncIndex)
	{
		case 0:  return m_pQuote->m_pfDynaData[INDEX_ZXJ];	//���¼�
		case 1:  
			if (m_pQuote->m_pfDynaData[INDEX_BJ1]>1)
				return m_pQuote->m_pfDynaData[INDEX_BJ1];	//���
			else
				return m_pQuote->m_pfDynaData[INDEX_ZXJ];	//��ͣ
		case 2:  
			if (m_pQuote->m_pfDynaData[INDEX_SJ1]>1)
				return m_pQuote->m_pfDynaData[INDEX_SJ1];	//����
			else
				return m_pQuote->m_pfDynaData[INDEX_ZXJ];	//��ͣ
		case 3:  return m_pQuote->m_pfDynaData[INDEX_PJJ];	//����
		case 4:  return m_CurrOderPrice;					//��ǰ����ί�м�
		case 5:  return m_pQuote->m_pfDynaData[INDEX_ZTJ];	//��ͣ��
		case 6:  return m_pQuote->m_pfDynaData[INDEX_DTJ];	//��ͣ��
		case 7:  return m_pQuote->m_pfDynaData[INDEX_ZGJ];	//��߼�
		case 8:  return m_pQuote->m_pfDynaData[INDEX_ZDJ];	//��ͼ�
		case 9:  return m_pQuote->m_pfDynaData[INDEX_ZJS];	//������
		case 10: return m_pQuote->m_pfDynaData[INDEX_KPJ];	//���̼�
		case 11: return m_pQuote->m_pfDynaData[INDEX_BL1];	//����
		case 12: return m_pQuote->m_pfDynaData[INDEX_SL1];	//����
		case 13: 
			{
				int m_BeginInd=BeginInd;
				int m_EndInd=EndInd;
				int Index= GetCurrPeriod(GetOperand(NULL, BeginInd, EndInd).nData);
				if (Index<0)
					AddError(LoadRC::load_str(IDS_UNVAILD_TICKDATA).c_str(), m_BeginInd, m_EndInd);
				else
					return Index;
			}
		default: 
			AddError(LoadRC::load_str(IDS_UNVAILD_FUNCNAME).c_str(), BeginInd-1, BeginInd-1);
			return 0;
	}
}

int CExpressionEx::GetCurrPeriod(int DataType)
{
	if (DataType<RAW_DAY_DATA || DataType>DAYX_DATA)
		return -1;
	CStkHisData * pStkHisData=m_pQuote->CreateHisData(DataType);
	if (! pStkHisData || pStkHisData->m_nNumData<=0)
	{
		if (pStkHisData)
			delete pStkHisData;
		return -1;
	}

	int nIndex = pStkHisData->SearchIndexByTime(m_pQuote->m_timeDynaData, TRUE);
	if ( nIndex < 0 )
		nIndex = 0;
	 else if ( nIndex >= pStkHisData->m_nNumData )
		nIndex = pStkHisData->m_nNumData - 1;

	delete pStkHisData;
	return nIndex;
}