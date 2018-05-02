#pragma once


#include <windows.h>
#include <stdio.h>

#include <iostream>
#include <vector>
using namespace std;

#define invoke invoke1
/*
ret方式可以返回不同类型的值，但其不方便作FreeLibrary处理
return方式可以处理FreeLibrary
不同类型的返回值会保存在不同的寄存器之中：
┌───────────────────────────────────────────────┬───────────────────────────────┐
│返回类型										│寄存位置						│
│───────────────────────────────────────────────┼───────────────────────────────│
│Char、Byte										│AL 寄存器						│
│SmallInt、Word									│AX 寄存器						│
│Integer、LongWord、AnsiString、Pointer、Class	│EAX 寄存器						│
│Real48											│EAX 寄存器中是栈内返回值的指针	│
│Int64											│EDX、EAX  寄存器对				│
│Single、Double、Extended、Comp					│栈首寄存器 ST(0)				│
│短字符串或变体类型								│在@Result指向的临时位置中返回	│
└───────────────────────────────────────────────┴───────────────────────────────┘
*/


#define NAKED __declspec(naked)

//1.//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//内存拷贝
#define MEMCPY(pDest, pSource, S)   \
	__asm  mov    edi, pDest		\
	__asm  mov    esi, pSource		\
	__asm  mov    ecx, S			\
	__asm  xor    eax, eax			\
	__asm  cld						\
	__asm  lodsb					\
	__asm  stosb					\
	__asm  dec    ecx				\
	__asm  test   ecx, ecx			\
	__asm  _emit  0x75				\
	__asm  _emit  0xF9  


double NAKED invoke1(const char* lib, const char* funname, ...)
{
	PULONG stack, arg;
	ULONG argsize, bak_esp, fun;
	HMODULE hlib;
	double result;
	__asm
	{
		mov stack, esp
	}
	//找到调用invoke的返回地址，add esp,0xx 即可计算参数个数
	if(*(PUSHORT)stack[0] == 0xC483) 
	{
		argsize = *(PUCHAR)(stack[0] + 2) - 8;//invoke参数大小 减去8（2*4） 就是要调用函数的参数的大小
		arg = &stack[3];
	}
	else
	{
		__asm mov eax, -1
		goto RET;
	}

	hlib = LoadLibrary((PCHAR)stack[1]);
	fun = (ULONG)GetProcAddress(hlib, (PCHAR)stack[2]);
	__asm
	{
		mov bak_esp, esp
		sub esp, argsize
		MEMCPY(esp, arg, argsize)
		call fun		
		mov esp, bak_esp		//保证C调用的堆栈平衡
	}
	if (hlib)
		FreeLibrary(hlib);

RET:
	__asm
	{
		ret
	}
}

//2.//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double ivk(void * fun, ULONG count, va_list & ap)
{
	ULONG bak_esp;
	vector<ULONG> args;
	
	ULONG i;
	for (i=0; i< count; i++)
		args.push_back(va_arg(ap, ULONG));

	__asm mov bak_esp, esp

	while (!args.empty())
	{
		i = args.back();
		args.pop_back();
		__asm push i
	}

	__asm
	{
		call [fun];
		mov esp, bak_esp
	}
}

//StdInvoke
double invoke2(const char* lib, const char* FucName, ...)
{
	ULONG count, func;
	double  result;

	count = ((((*(int*)(*(int*)((int)&lib - 4)))&0xff0000) >> 16) / 4) - 2;

	HMODULE hlib = LoadLibrary(lib);
	func = (int)GetProcAddress(hlib, FucName);

	va_list ap;
	va_start(ap, FucName);
	result = ivk((void *)func, count, ap);
	va_end(ap);

	if (hlib) FreeLibrary(hlib);

	return result;
}

//3.//////////////////////////////////////////////////////////////////////////////////////////
unsigned long NAKED _calleresp()
{
	__asm mov eax, esp
	__asm add eax, 4
	__asm ret
}
//没有执行FreeLibrary
double NAKED _invoke(const char* libstr, const char* funcstr, ...)
{
	__asm 
	{
		mov		[esp-4]	, edi
		mov		edi		, [esp]
		mov		[eax]	, edi
		sub		eax		, 4
		mov		[eax]	, esp
		sub		eax		, 4
		mov		edi		, [esp-4]
		mov		[eax]	, edi
		mov		edi		, eax
		add		edi		, 4
		sub		eax		, 4
		mov		[eax]	, ecx
		add		esp		, 0x04
		mov		eax		, [esp]
		call	DWORD	  ptr[LoadLibrary]
		mov		ecx		, [esp]
		push	ecx
		push	eax
		call	DWORD	  ptr[GetProcAddress]
		add		esp		, 0x04
		call	eax
		mov		esp		, edi
		add		esp		, 4
		mov		ecx		, [esp]
		mov		esp		, [esp-4]
		mov		[esp]	, ecx 
		mov		esp		, edi
		mov		edi		, [esp-4]
		mov		ecx		, [esp-8]
		mov		esp		, [esp]
		ret
	}
}

#define invoke3(libstr, funcstr, ...) _invoke(libstr, funcstr, __VA_ARGS__, NULL, NULL, NULL, NULL, _calleresp())
#define INVOKE_TEST(libstr, funcstr, ...) printf("TEST:\t%s(%s)\tRET:\t%d\n\n", libstr, funcstr, invoke3(libstr, funcstr, __VA_ARGS__))
