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
	//将数字Value规范为最靠近的SubValue的整数倍
	static double GetMod(const double Value, const double SubValue)
	{
		if (SubValue!=0)
			return Round(Value / SubValue)*SubValue;
		else
			return Value;
	}
	//是否整除
	static bool IsDiv(const double Value, const double SubValue, const double ErrorDiff=1e-5)
	{
		double n = 0;
		double m = modf(abs(Value / SubValue), &n);
		return m < ErrorDiff || 1 - m < ErrorDiff; //误差使值变大或误差使值变小两钟情况处理
	}
	//获取整除的结果
	static int  DivResult(const double Value, const double SubValue, const double ErrorDiff=1e-5)
	{
		//正数？
		bool bplus=false;
		if(Value*SubValue>0)	bplus=true;

		double n = 0;
		double m = modf(abs(Value / SubValue), &n);
		int i=0;//返回0说明错误
		if(m < ErrorDiff)
			i=(int)n;
		else if(1 - m < ErrorDiff)
			i=(int)n+1;
		if(!bplus) i=-i;
		return i;
	}
	//得到分数报价时分母的长度 比如分母240的长度等于3
	static unsigned int GetDenominatorLength(int Denominator)
	{
		char cLength[11]={0};
		_itoa_s(Denominator,cLength,10);
		string strLength(cLength);
		return strLength.length();
	}
	//正则表达式匹配
	static bool MatchPattern(const char *pattern,const char *target)//正则匹配
	{
		cmatch res;  
		regex rx(pattern);  
		regex_search(target, res, rx);  
		return !res.empty();
	}
	//判断是否是合法的数字输入
	static bool IsValidNumber(const char * sValue, bool bOnlyInt=false, bool bCanNegative=false)//是否有效的数字
	{
		string pattern="^(\\d+)?(\\.)?(\\d+)?$";	//整数、小数   
		if (bOnlyInt)			
			pattern="^(\\d+)?$";					//只允许为整数    
		if (bCanNegative)
			pattern.insert(1, "(-)?");				//允许为负  "^[-+]?(\\d+)?(\\.)?(\\d+)?$"

		return MatchPattern(pattern.c_str(), sValue);
	}
	//四舍五入输入值，判断是否是有效的价输入，否 调整到最小变动家的倍数
	static bool IfEffectivelyPrice(const SOFCommodityInf &CommodityInf,double & nInputPrice) 
	{
		if(CommodityInf.bIfFractionPrice)//分数的情况下  分子 分母都是整数，所以只要限定不能输入小数就行了,这里只需要检查你输入分子的整数是否已经大于分母了，大于了应该进位
		{
			if(0==static_cast<int>(CommodityInf.dMiniChangePrice)/CommodityInf.iPrecision )    //分子没有大于分母
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
	//四舍五入输入值，判断是否是有效的价输入  
	static bool IfEffectivelyInput(const SOFCommodityInf &CommodityInf,const double & nInputPrice)
	{
		if(CommodityInf.bIfFractionPrice)//分数的情况下  分子 分母都是整数，所以只要限定不能输入小数就行了,这里只需要检查你输入分子的整数是否已经大于分母了，大于了应该进位
		{
			if(0==static_cast<int>(CommodityInf.dMiniChangePrice)/CommodityInf.iPrecision )   //分子没有大于分母
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

	//把小数价格转为分数 denominator分母 
	static bool DecimalsToFractions( double inPrice,int denominator,SFractionPrice & out)
	{
		char cMolecular[11]={0};
		int   iInteger=0;
		double fDecimal=0;							//分数的整数部分和小数部分
		iInteger=(int)inPrice;
		fDecimal=inPrice-(float)iInteger;
		double fPrice=fDecimal*denominator;
		sprintf_s(cMolecular,"%0.*lf",0,fPrice);	//四舍五入分子部分 ,分母部分就是Precision
		
		out.iInteger=iInteger;						//整数部分
		out.iMolecules=(int)atof(cMolecular);		//分子 分母就是Precision MiniChangePrice是每次要变动的单位
		out.idenominator=denominator;
		return true;
	}
	//把分数价格转为小数
	static bool FractionsToDecimals(const SFractionPrice & in,double &outPirce)
	{
		outPirce=in.iInteger+in.iMolecules/in.idenominator;
		return true;
	}
	//把double价格转为字符串
	static bool DecimalsToStr(const TContractKey &ContractKey,double dOrginPrice,string &strPrice)
	{
		SOFCommodityInf temp;
		char cc[101] = { 0 };
		if (!CConvert::GetCommodityInf(ContractKey, temp))
		{
			//交易取不到 取行情
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
	//把字符串价格转为double
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
	//把字符串价格转为double
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
	//判断两浮点数是否相等
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