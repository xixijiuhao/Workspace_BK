/* idea.h */

#ifndef _IDEA_DOT_H
#define _IDEA_DOT_H

#include <stdio.h>
#include <Windows.h>
/*
#include <time.h>
#include <process.h>
#include <io.h>
#include <string.h>
*/
//#include <conio.h>

#define IDEAKEYSIZE 16
#define IDEABLOCKSIZE 8
#define ROUNDS	8
#define KEYLEN	(6*ROUNDS+4)

#define low16(x) ((x)&0xffff)

typedef unsigned short word16;
typedef unsigned int word32;

typedef unsigned short uint16;
typedef unsigned short IDEAkey[KEYLEN];

/*IDEA Algorithm functions
void en_key_idea(word16 userkey[8],IDEAkey Z);
void de_key_idea(IDEAkey Z, IDEAkey DK);
void cipher_idea(word16 in[4],word16 out[4],IDEAkey Z);
uint16 inv(uint16 x);
uint16 mul(uint16 a,uint16 b);
 */
/*buffer handling functions*/
typedef struct tagIDEA_KEY{
  char userKey[16];
  IDEAkey CipherKey,DeCipherKey;
}IDEA_KEY,*PIDEA_KEY;

void SetCipher( PIDEA_KEY key,const char*userKey,int KeyLen);
void cipher_buffer(const char *in,char *out,int len,PIDEA_KEY key);
void decipher_buffer(const char *in,char *out,int len,PIDEA_KEY key);

#endif




