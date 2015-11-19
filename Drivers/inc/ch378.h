#ifndef __CH378_H__
#define __CH378_H__
#include "ch378inc.h"
#include "includes.h"

u8 mInitCH378Device( void ) ;
void xWriteCH378Data( UINT8 mData ) ;
void xWriteCH378Cmd( UINT8 mCmd );
u8  xReadCH378Data( void  );
void	mCH378Interrupt( void );

#endif

