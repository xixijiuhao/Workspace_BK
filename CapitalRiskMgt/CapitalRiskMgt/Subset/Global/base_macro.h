#pragma once

//ȡ�ýṹ��type��field�ֶε�ƫ�Ƶ�ַ
#define FPOS(type, field)	(dword)&((type *)0)->field 

//ȡ�ýṹ��type��field��ռ�õ��ֽ��� 
#define FSIZ(type, field)	sizeof(((type *)0)->field) 

//����һ��n�Ĵ���x����С����
#define RND_H(x, n)			((((x) + (n) - 1) / (n) ) * (n) )

//����һ��n��С��x�������
#define RND_L(x, n)			((((x) - (n) - 1) / (n) ) * (n) )

//����һ����x��ӽ���n�ı���
#define RND(x, n)			((x) % (n) >= (n) / 2) ? RND_H(x, n) : RND_L(x, n) 

//������xΪ�ף�y�Ķ���
#define LOG(x, y)			(log(x) / log(y))