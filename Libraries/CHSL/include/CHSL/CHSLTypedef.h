#pragma once



#ifndef CHSL_NOTYPES

typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ullong;

typedef const char* cstr;

#endif



#ifndef CHSL_NOSTRING

#include <string>

using std::string;
using std::wstring;

#endif
