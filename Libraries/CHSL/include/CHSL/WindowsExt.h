#pragma once

/* CHSL

    |	Windows.h wrapper that defines many flags to clean up the mess.

*/









#define _WIN32_WINNT 0x0601 // Target Windows 7 or later
#include <sdkddkver.h>

#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
//#define NORASTEROPS
#define NOPATOM
//#define NOCLIPBOARD
//#define NOCOLOR
#define NODRAWTEXT
#define NOKERNEL
//#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE

#define OEMRESOURCE
#define STRICT

#undef UNICODE

#include <Windows.h>