//��  �ܣ�CSV��ʽ��DSV��ʽ���ַ�������ͽ�������
//�����ˣ�������

#pragma once

#include "Global.h"


//DSV���﷨�ǳ��򵥣�ֻ�����������㣺
//1. ͨ��˫����'||'��Ϊ�ֶηָ���
//2. ���ֶ�ֵ�е�'|'�滻Ϊ'_|'����ת��
class TDsv
{
	//���
	static void StringsToDsv(__in const Strings & items, __out string & line)
	{
		for (size_t i = 0; i < items.size(); i++)
		{
			line.append(StrFun::replace(items[i].c_str(), "|", "_|"));
			if (i < items.size() - 1)
				line.append("||");
			else
				line.append("\r\n");
		}
	}
	//����
	static void DsvToStrings(__in const char * line, __out Strings & items)
	{
		if (line == nullptr || *line == 0)
			return;

		Strings strs;
		StrFun::SplitString(line, strs, "||");
		for (size_t i = 0; i < strs.size(); i++)
			items.push_back(StrFun::replace(strs[i].c_str(), "_|", "|"));
	}
};


//�ѵ�ǰ�������ݵ���ȥ TXT CVS DVS
//CSV��ʽ�淶��
// 1 ��ͷ�����գ�����Ϊ��λ
// 2 �ɺ��򲻺�����������������ļ���һ��
// 3 һ�����ݲ����У��޿���
// 4 �Զ������ָ�������Ϊ��ҲҪ�������ڣ�����ǰ��Ŀո�ᱻ����
// 5 ����������ڶ��ţ����ֶ���˫����������
// 6 ����������ڿո񣬸��ֶ���˫����������
// 7 ����������ڻ��з������ֶ���˫����������
// 8 ����������ڰ��������Ӧ�滻��˫������ת�壬�������ֶ���˫����������
// 9 �ļ���дʱ���ţ����Ų���������
// 10�����ʽ���ޣ���ΪASCII��Unicode��������
class TCsv
{
public:
	//���
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
	//����
	static void CsvToStrings(__in const char * line, __out Strings & items)
	{
		//״̬����
		enum FieldStateType
		{
			fstStart			, //���ֶο�ʼ		
			fstNonQuotes		, //�������ֶ�		
			fstQuotes			, //�����ֶ�		
			fstSeparator		, //�ֶηָ�		
			fstQuoteInQuotes	  //�����ֶ��е�����		
		};

		//��ʼ״̬
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
					else if (ch == '\r' || ch == '\n')	//�﷨����ǿ�ƽ���
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
					if (ch == ',')				//�����ֶαպ�
					{
						items.push_back(item);
						state = fstSeparator;
						item.clear();
					}
					else if (ch == '\r')		//�����ֶαպ�
					{
						items.push_back(item);
						return;
					}
					else if (ch == '"')			//ת��
					{
						item.push_back(ch);
						state = fstQuotes;
					}
					else //�﷨����ת���ַ� \" �������ת�� �� �����ֶν�β����û�н����ֶηָ���, ǿ�ƽ���
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