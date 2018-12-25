#pragma once


#include <windows.h>
#include <stdio.h>

#include <iostream>
#include <vector>
using namespace std;

#define invoke invoke1
/*
ret��ʽ���Է��ز�ͬ���͵�ֵ�����䲻������FreeLibrary����
return��ʽ���Դ���FreeLibrary
��ͬ���͵ķ���ֵ�ᱣ���ڲ�ͬ�ļĴ���֮�У�
�������������������������������������������������������������������������������������������������Щ���������������������������������������������������������������
����������										���Ĵ�λ��						��
�������������������������������������������������������������������������������������������������੤��������������������������������������������������������������
��Char��Byte										��AL �Ĵ���						��
��SmallInt��Word									��AX �Ĵ���						��
��Integer��LongWord��AnsiString��Pointer��Class	��EAX �Ĵ���						��
��Real48											��EAX �Ĵ�������ջ�ڷ���ֵ��ָ��	��
��Int64											��EDX��EAX  �Ĵ�����				��
��Single��Double��Extended��Comp					��ջ�׼Ĵ��� ST(0)				��
�����ַ������������								����@Resultָ�����ʱλ���з���	��
�������������������������������������������������������������������������������������������������ة���������������������������������������������������������������
*/


#define NAKED __declspec(naked)

//1.//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//�ڴ濽��
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
	//�ҵ�����invoke�ķ��ص�ַ��add esp,0xx ���ɼ����������
	if(*(PUSHORT)stack[0] == 0xC483) 
	{
		argsize = *(PUCHAR)(stack[0] + 2) - 8;//invoke������С ��ȥ8��2*4�� ����Ҫ���ú����Ĳ����Ĵ�С
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
		mov esp, bak_esp		//��֤C���õĶ�ջƽ��
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
//û��ִ��FreeLibrary
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
