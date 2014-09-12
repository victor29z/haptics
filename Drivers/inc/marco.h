/***************************************************************************\
	Copyright (c) 2004-2007 threewater@up-tech.com, All rights reserved.
	by threewter	2004.4.26
\***************************************************************************/
	

/***************************************************************************\
    #说明: 常量和宏定义
	----------------------------------  Bug  --------------------------------------

	----------------------------------  TODO list  --------------------------------------

	----------------------------------修正--------------------------------------
	2004-4-26	创建
	2005-12-31	添加环行缓冲区管理的宏定义

\***************************************************************************/
#ifndef __MARCO_H__
#define __MARCO_H__

#define U32 unsigned int
#define U16 unsigned short
#define S32 int
#define S16 short int
#define U8  unsigned char
#define S8  char
#define BOOL unsigned char

#ifdef USE_CUSTOM_TYPES
typedef   signed char     int8_t;
typedef   signed short int    int16_t;
typedef   signed long int    int32_t;

typedef   unsigned char   uint8_t;
typedef   unsigned short int uint16_t;
typedef   unsigned long int  uint32_t;
#endif

#ifndef NULL
#define NULL 0
#endif


#define TRUE 	1
#define FALSE 	0
#define OK		0
#define FAIL		-1

#define SIZE_1KB		1024ul
#define SIZE_1MB	(SIZE_1KB*SIZE_1KB)
#define SIZE_1GB		(SIZE_1MB*SIZE_1KB)


#define NumberOfArray(array)	(sizeof(array)/sizeof(array[0]))
#define min(x1,x2)	((x1<x2)? x1:x2)
#define max(x1,x2)	((x1>x2)? x1:x2)
#define ABS(a)		((a)>=0?(a):(-(a)))

#define BYTE_MASK_BIT0		0x1
#define BYTE_MASK_BIT1		0x2
#define BYTE_MASK_BIT2		0x4
#define BYTE_MASK_BIT3		0x8
#define BYTE_MASK_BIT4		0x10
#define BYTE_MASK_BIT5		0x20
#define BYTE_MASK_BIT6		0x40
#define BYTE_MASK_BIT7		0x80


#define HEXCHR2BYTE(x,y)	if(x >= '0' && x<='9')					\
									y = (x - 0x30) ;					\
								else{									\
									switch(x){							\
										case 'a':case'A':				\
										y = 0xa;						\
										break;							\
										case 'b':case'B':				\
										y = 0xb;						\
										break;							\
										case 'c':case'C':				\
										y = 0xc;						\
										break;							\
										case 'd':case'D':				\
										y = 0xd;						\
										break;							\
										case 'e':case'E':				\
										y = 0xe;						\
										break;							\
										case 'f':case'F':				\
										y = 0xf;						\
										break;							\
									}									\
								}






//获得number中最低位的1的位置，通过n返回
#define GetPosOfFirstOne(number, n)	do{unsigned int tmp=1; for(n=1;(number|tmp)!=number;n++,tmp<<=1);}while(0)

//环行缓冲区下标增加，mod必须为2的幂
#define RoundINC(data, mod)	do{(data)++; (data)&=((mod)-1);}while(0)

/********************************************************\
	返回环行缓冲区元素个数，mod必须为2的幂，
	head, 填充的环行缓冲区头
	tail, 填充的环行缓冲区尾
\********************************************************/
//#define RoundCount(head, tail, mod)	(((head)-(tail))&((mod)-1))
#define RoundCount(head, tail, mod)	(((head)+(mod)-(tail))%(mod))

/********************************************************\
	返回环行缓冲区空闲元素个数，mod必须为2的幂，
	head, 填充的环行缓冲区头
	tail, 填充的环行缓冲区尾
\********************************************************/
#define RoundFreeCount(head, tail, mod)	((mod)-RoundCount(head, tail, mod)-1)

/********************************************************\
	从环行缓冲区复制数据，mod必须为2的幂，
	head, 填充的环行缓冲区头
	tail, 填充的环行缓冲区尾
\********************************************************/
#define RoundmemCpyFrom(dst, src, pos, n, mod)	do{	int i;\
	for(i=0;i<n;i++){ \
		dst[i]=src[pos]; \
		RoundINC(pos, mod); }}while(0)

/********************************************************\
	向环行缓冲区复制数据，mod必须为2的幂，
	head, 填充的环行缓冲区头
	tail, 填充的环行缓冲区尾
\********************************************************/
#define RoundmemCpyTo(dst, src, pos, n, mod)	do{	int i;\
	for(i=0;i<n;i++){ \
		dst[pos]=src[i]; \
		RoundINC(pos, mod); }}while(0)

#include "lib_def.h"

#endif
