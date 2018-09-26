#pragma once
#include <algorithm>
#include <regex>
#include "share\TradeQuoteString.h"

class CGeneralGlobal
{
public:
	static long Round(double Value)
	{
		if (Value>0)
			return (long)(Value+0.5);	
		else
			return (long)(Value-0.5);
	}
	//������Value�淶Ϊ�����SubValue��������
	static double GetMod(const double Value, const double SubValue)
	{
		if (SubValue!=0)
			return Round(Value / SubValue)*SubValue;
		else
			return Value;
	}
	//�Ƿ�����
	static bool IsDiv(const double Value, const double SubValue, const double ErrorDiff=1e-5)
	{
		double n = 0;
		double m = modf(abs(Value / SubValue), &n);
		return m < ErrorDiff || 1 - m < ErrorDiff; //���ʹֵ�������ʹֵ��С�����������
	}
	//��ȡ�����Ľ��
	static int  DivResult(const double Value, const double SubValue, const double ErrorDiff=1e-5)
	{
		//������
		bool bplus=false;
		if(Value*SubValue>0)	bplus=true;

		double n = 0;
		double m = modf(abs(Value / SubValue), &n);
		int i=0;//����0˵������
		if(m < ErrorDiff)
			i=(int)n;
		else if(1 - m < ErrorDiff)
			i=(int)n+1;
		if(!bplus) i=-i;
		return i;
	}
	//�õ���������ʱ��ĸ�ĳ��� �����ĸ240�ĳ��ȵ���3
	static unsigned int GetDenominatorLength(int Denominator)
	{
		char cLength[11]={0};
		_itoa_s(Denominator,cLength,10);
		string strLength(cLength);
		return strLength.length();
	}
	//������ʽƥ��
	static bool MatchPattern(const char *pattern,const char *target)//����ƥ��
	{
		cmatch res;  
		regex rx(pattern);  
		regex_search(target, res, rx);  
		return !res.empty();
	}
	//�ж��Ƿ��ǺϷ�����������
	static bool IsValidNumber(const char * sValue, bool bOnlyInt=false, bool bCanNegative=false)//�Ƿ���Ч������
	{
		string pattern="^(\\d+)?(\\.)?(\\d+)?$";	//������С��   
		if (bOnlyInt)			
			pattern="^(\\d+)?$";					//ֻ����Ϊ����    
		if (bCanNegative)
			pattern.insert(1, "(-)?");				//����Ϊ��  "^[-+]?(\\d+)?(\\.)?(\\d+)?$"

		return MatchPattern(pattern.c_str(), sValue);
	}
	//������������ֵ���ж��Ƿ�����Ч�ļ����룬�� ��������С�䶯�ҵı���
	static bool IfEffectivelyPrice(const SOFCommodityInf &CommodityInf,double & nInputPrice) 
	{
		if(CommodityInf.bIfFractionPrice)//�����������  ���� ��ĸ��������������ֻҪ�޶���������С��������,����ֻ��Ҫ�����������ӵ������Ƿ��Ѿ����ڷ�ĸ�ˣ�������Ӧ�ý�λ
		{
			if(0==static_cast<int>(CommodityInf.dMiniChangePrice)/CommodityInf.iPrecision )    //����û�д��ڷ�ĸ
				return true;
		}
		else
		{
			if( !IsDiv(nInputPrice,CommodityInf.dMiniChangePrice) )
			{
				nInputPrice=GetMod(nInputPrice,CommodityInf.dMiniChangePrice);
				return true;
			}
			else
				return false;
		}
		return true;
	}
	//������������ֵ���ж��Ƿ�����Ч�ļ�����  
	static bool IfEffectivelyInput(const SOFCommodityInf &CommodityInf,const double & nInputPrice)
	{
		if(CommodityInf.bIfFractionPrice)//�����������  ���� ��ĸ��������������ֻҪ�޶���������С��������,����ֻ��Ҫ�����������ӵ������Ƿ��Ѿ����ڷ�ĸ�ˣ�������Ӧ�ý�λ
		{
			if(0==static_cast<int>(CommodityInf.dMiniChangePrice)/CommodityInf.iPrecision )   //����û�д��ڷ�ĸ
				return true;
			else 
				return false;
		}
		else
		{
			if( IsDiv(nInputPrice,CommodityInf.dMiniChangePrice) )
				return true;
			else
				return false;
		}
		return true;
	}

	//��С���۸�תΪ���� denominator��ĸ 
	static bool DecimalsToFractions( double inPrice,int denominator,SFractionPrice & out)
	{
		char cMolecular[11]={0};
		int   iInteger=0;
		double fDecimal=0;							//�������������ֺ�С������
		iInteger=(int)inPrice;
		fDecimal=inPrice-(float)iInteger;
		double fPrice=fDecimal*denominator;
		sprintf_s(cMolecular,"%0.*lf",0,fPrice);	//����������Ӳ��� ,��ĸ���־���Precision
		
		out.iInteger=iInteger;						//��������
		out.iMolecules=(int)atof(cMolecular);		//���� ��ĸ����Precision MiniChangePrice��ÿ��Ҫ�䶯�ĵ�λ
		out.idenominator=denominator;
		return true;
	}
	//�ѷ����۸�תΪС��
	static bool FractionsToDecimals(const SFractionPrice & in,double &outPirce)
	{
		outPirce=in.iInteger+in.iMolecules/in.idenominator;
		return true;
	}
	//��double�۸�תΪ�ַ���
	static bool DecimalsToStr(const TContractKey &ContractKey,double dOrginPrice,string &strPrice)
	{
		SOFCommodityInf temp;
		char cc[101] = { 0 };
		if (!CConvert::GetCommodityInf(ContractKey, temp))
		{
			//����ȡ���� ȡ����
			string str;
			CConvert::TContractKeyToTContractNo(ContractKey,str);
			if (!CConvert::GetQuoteCommodityInf(str.c_str(), temp))
				return false;
		}

		if(temp.bIfFractionPrice)
		{
			SFractionPrice FracPrice;
			DecimalsToFractions(dOrginPrice,temp.iPrecision,FracPrice);
			sprintf_s(cc,"%d + %.*d/%d", FracPrice.iInteger, CGeneralGlobal::GetDenominatorLength(temp.iPrecision), FracPrice.iMolecules, temp.iPrecision);
		}
		else
			sprintf_s(cc, "%0.*lf", temp.iPrecision, dOrginPrice);
		strPrice = cc;
		return true;
	}
	//���ַ����۸�תΪdouble
	static bool StrToDecimals(const TContractKey &ContractKey,const string &strPrice,double & dOutPrice)
	{
		SOFCommodityInf temp;
		if (!CConvert::GetCommodityInf(ContractKey, temp)) return false;

		if(temp.bIfFractionPrice)
		{
			SFractionPrice FracPrice;
			sscanf_s(strPrice.c_str(),"%d + %d/%d",&FracPrice.iInteger,&FracPrice.iMolecules,&FracPrice.idenominator);
			dOutPrice=FracPrice.iInteger+(FracPrice.iMolecules/FracPrice.idenominator);
		}
		else
			dOutPrice=atof(strPrice.c_str());
		return true;
	}
	//���ַ����۸�תΪdouble
	static bool StrToDecimals2(const string &strPrice,double & dOutPrice)
	{
		if(-1!=strPrice.find('/'))
		{
			SFractionPrice FracPrice;
			sscanf_s(strPrice.c_str(),"%d + %d/%d",&FracPrice.iInteger,&FracPrice.iMolecules,&FracPrice.idenominator);
			dOutPrice=FracPrice.iInteger+(FracPrice.iMolecules/FracPrice.idenominator);
		}
		else
			dOutPrice=atof(strPrice.c_str());
		return true;
	}
	//�ж����������Ƿ����
	static bool DoubleSameValue(const double d1, const double d2, double Epsilon)
	{
		if (Epsilon == 0)
			Epsilon = max(min(abs(d1), abs(d2)) * (1e-15)*1000, (1e-15)*1000);
		if (d1 > d2)
			return (d1 - d2) <= Epsilon;
		else
			return (d2 - d1) <= Epsilon;
	};
};