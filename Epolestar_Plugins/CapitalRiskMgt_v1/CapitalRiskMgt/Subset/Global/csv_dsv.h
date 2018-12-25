//功  能：CSV格式和DSV格式的字符串打包和解析函数
//创建人：刘明祥

#pragma once

#include "Global.h"


//DSV的语法非常简单，只包括以下两点：
//1. 通过双竖线'||'作为字段分隔符
//2. 把字段值中的'|'替换为'|_'进行转义
//其实分割符中的'|'和'_'可以是任意两个不同的字符
class TDsv
{
	//打包
	static void StringsToDsv(__in const Strings & items, __out string & line)
	{
		for (size_t i = 0; i < items.size(); i++)
		{
			line.append(StrFun::replace(items[i].c_str(), "|", "|_"));
			if (i < items.size() - 1)
				line.append("||");
			else
				line.append("\r\n");
		}
	}
	//解析
	static void DsvToStrings(__in const char * line, __out Strings & items)
	{
		if (line == nullptr || *line == 0)
			return;

		Strings strs;
		StrFun::SplitString(line, strs, "||");
		for (size_t i = 0; i < strs.size(); i++)
			items.push_back(StrFun::replace(strs[i].c_str(), "|_", "|"));
	}
};


//把当前表格的内容导出去 TXT CVS DVS
//CSV格式规范：
// 1 开头不留空，以行为单位
// 2 可含或不含列名，含列名则居文件第一行
// 3 一行数据不垮行，无空行
// 4 以逗号作分隔符，列为空也要表达其存在，逗号前后的空格会被忽略
// 5 列内容如存在逗号，该字段用双引号括起来
// 6 列内容如存在空格，该字段用双引号括起来
// 7 列内容如存在换行符，该字段用双引号括起来
// 8 列内容如存在半角引号则应替换成双引号来转义，并将该字段用双引号括起来
// 9 文件读写时引号，逗号操作规则互逆
// 10内码格式不限，可为ASCII、Unicode或者其他
class TCsv
{
public:
	//打包
	static void StringsToCsv(__in const Strings & items, __out string & line)
	{
		for (size_t i = 0; i < items.size(); i++)
		{
			line.append(1, '"');
			line.append(StrFun::replace(items[i].c_str(), "\"", "\"\""));
			if (i < items.size() - 1)
				line.append("\",");
			else
				line.append("\"\r\n");
		}
	}
	//解析
	static void CsvToStrings(__in const char * line, __out Strings & items)
	{
		//状态定义
		enum FieldStateType
		{
			fstStart			, //新字段开始		
			fstNonQuotes		, //非引号字段		
			fstQuotes			, //引号字段		
			fstSeparator		, //字段分隔		
			fstQuoteInQuotes	  //引号字段中的引号		
		};

		//初始状态
		FieldStateType state = fstStart;

		string item;
		const char * p = line;
		while (*p)
		{
			const char & ch = *p++;

			switch (state)
			{
				case fstStart:
				{
					if (ch == '"')
						state = fstQuotes;
					else if (ch == ',')
					{
						items.push_back("");
						state = fstSeparator;
					}
					else if (ch == '\r' || ch == '\n')	//语法错误：强制结束
					{
						items.push_back(item);
						return;
					}
					else
					{
						item.push_back(ch);
						state = fstNonQuotes;
					}
				}
				break;

				case fstNonQuotes:
				{
					if (ch == ',')
					{
						items.push_back(item);
						state = fstSeparator;
						item.clear();
					}
					else if (ch == '\r')
					{
						items.push_back(item);
						return;
					}
					else
						item.push_back(ch);
				}
				break;

				case fstQuotes:
				{
					if (ch == '"')
						state = fstQuoteInQuotes;
					else
						item.push_back(ch);
				}
				break;

				case fstSeparator:
				{
					if (ch == ',')
						items.push_back("");
					else if (ch == '"')
					{
						item.clear();
						state = fstQuotes;
					}
					else if (ch == '\r')
					{
						items.push_back("");
						return;
					}
					else
					{
						item.push_back(ch);
						state = fstNonQuotes;
					}
				}
				break;

				case fstQuoteInQuotes:
				{
					if (ch == ',')				//引号字段闭合
					{
						items.push_back(item);
						state = fstSeparator;
						item.clear();
					}
					else if (ch == '\r')		//引号字段闭合
					{
						items.push_back(item);
						return;
					}
					else if (ch == '"')			//转义
					{
						item.push_back(ch);
						state = fstQuotes;
					}
					else //语法错误：转义字符 \" 不能完成转义 或 引号字段结尾引号没有紧贴字段分隔符, 强制结束
					{
						items.push_back(item);
						state = fstSeparator;
						item.clear();
					}

				}
				break;
			}
		}//while


		if (state != fstStart)
			items.push_back(item);
	}
};