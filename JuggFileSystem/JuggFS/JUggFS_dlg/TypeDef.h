#ifndef _TYPEDEF__
#define _TYPEDEF__

#include <wchar.h>
//======================
//basic type redefinition
//======================

typedef       char                Char;
typedef       unsigned char       UChar;
typedef       short               Short;
typedef       unsigned short      UShort;
typedef	long int			Int;
typedef	unsigned long int	UInt;

typedef wchar_t WChar;
typedef wchar_t * JPWStr;
typedef const wchar_t * JPCWStr;

typedef int Bool;

#ifndef False
#define False 0
#endif

#ifndef True
#define True 1
#endif

#ifdef _MSC_VER
typedef       __int64             Int64;

#if _MSC_VER <= 1200 // MS VC6
typedef       __int64             UInt64;   // MS VC6 does not support unsigned __int64 to double conversion
#else
typedef       unsigned __int64    UInt64;
#endif

#else

typedef       long long           Int64;
typedef       unsigned long long  UInt64;

#endif

#endif