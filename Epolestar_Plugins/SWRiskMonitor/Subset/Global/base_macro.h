#pragma once

//取得结构体type中field字段的偏移地址
#define FPOS(type, field)	(dword)&((type *)0)->field 

//取得结构体type中field所占用的字节数 
#define FSIZ(type, field)	sizeof(((type *)0)->field) 

//返回一个n的大于x的最小倍数
#define RND_H(x, n)			((((x) + (n) - 1) / (n) ) * (n) )

//返回一个n的小于x的最大倍数
#define RND_L(x, n)			((((x) - (n) - 1) / (n) ) * (n) )

//返回一个与x最接近的n的倍数
#define RND(x, n)			((x) % (n) >= (n) / 2) ? RND_H(x, n) : RND_L(x, n) 

//返回以x为底，y的对数
#define LOG(x, y)			(log(x) / log(y))