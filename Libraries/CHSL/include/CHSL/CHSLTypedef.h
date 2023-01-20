#pragma once



#ifndef CHSL_NOTYPES

#include <cstdint>

typedef uint8_t byte;
typedef uint16_t ushort;
typedef uint32_t uint;
typedef uint64_t uint64;

typedef const char* cstr;

#endif



#ifndef CHSL_NOSTRING

#include <string>

using std::string;
using std::wstring;

#endif
