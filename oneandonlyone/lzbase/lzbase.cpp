#include "lzbase.h"
#include <Windows.h>
#include <tchar.h>

#ifdef _DEBUG
void OutputInVS(wchar_t * format, ...)
{
	va_list args;
	va_start(args, format);
	WCHAR buf[2560];
	_vstprintf_s(buf, 2560, format, args);
	::OutputDebugString(buf);
	va_end(args);
}
#else
void OutputInVS(wchar_t * format, ...)
{ }
#endif
