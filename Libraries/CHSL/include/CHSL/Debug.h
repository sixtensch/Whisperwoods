#pragma once

/* CHSL

	|	Debugging utilities
	|	
	|	Types Exception, ExceptionGeneral, ExceptionWindows
	|
	|	Macros EXC, EXC_TEMP, EXC_HRCHECK, EXC_HRLAST, EXC_COMCHECK, EXC_COMINFO, EXC_COMCHECKINFO
	|
	|	- Debugging exception types with good generalized code information.
	|	- Macros for shorthand exception throwing.
	|	- Macros for catching errors and messages.

*/









#include <exception>
#include <string>
#include "CHSLTypedef.h"

#if defined(CHSL_DX) || defined(CHSLCOMPILE)
#include <vector>

namespace cs
{

	void initDXGI();

	namespace  dxgiInfo
	{

		void set();
		std::vector<std::string> getMessages();
		void deInit();

	}
}

#endif

namespace cs
{

	class Exception : public std::exception
	{
	protected:
		// Mutable so that it can be written to from what()
		mutable std::string whatBuffer;

	private:
		std::string file, func;
		int line;

	public:
		Exception() = delete;
		Exception(cstr file, cstr func, int line);

		cstr what() const override;

		virtual cstr GetType() const;
		const std::string& GetFile() const;
		const std::string& GetFunc() const;
		int GetLine() const;
		std::string GetOriginString() const;
	};

	class ExceptionGeneral : public Exception
	{
	private:
		std::string str;

	public:
		ExceptionGeneral() = delete;
		ExceptionGeneral(cstr file, cstr func, int line, std::string string);

		cstr what() const override;

		virtual cstr GetType() const override;
		std::string GetString() const;
	};

#if defined(_WINDOWS_) || defined(CHSLCOMPILE)

#ifdef CHSLCOMPILE
	typedef long HRESULT;
#endif

	class ExceptionWindows : public Exception
	{
	private:
		HRESULT hr;
		std::vector<std::string> info;

	public:
		ExceptionWindows() = delete;
		ExceptionWindows(cstr file, cstr func, int line, HRESULT hResult);
		ExceptionWindows(cstr file, cstr func, int line, HRESULT hResult, std::vector<std::string> info);

		cstr what() const override;

		virtual cstr GetType() const override;
		HRESULT GetErrorCode() const;
		std::string GetErrorString() const;

		// Publicly avaliable helper functions
		static std::string TranslateHRESULT(HRESULT hResult);
		static std::string TranslateMessageArray(std::vector<std::string> messages);
	};

#endif

}

#if defined(CHSL_EXCEPT) || defined(CHSLCOMPILE)

#define EXC_TEMP()					throw cs::Exception(__FILE__, __FUNCTION__, __LINE__)
#define EXC(str)					throw cs::ExceptionGeneral(__FILE__, __FUNCTION__, __LINE__, str)

#if defined(_WINDOWS_) || defined(CHSLCOMPILE)
#define EXC_HRCHECK(hrcall)			{ HRESULT _hres = (hrcall); if (_hres != 0) { throw cs::ExceptionWindows(__FILE__, __FUNCTION__, __LINE__, _hres); } }
#define EXC_HRLAST()				{ HRESULT hres = GetLastError(); if (hres != 0) { throw cs::ExceptionWindows(__FILE__, __FUNCTION__, __LINE__, hres); } }
#endif

#if defined(CHSL_DX) || defined(CHSLCOMPILE)
#define EXC_COMCHECK(hrcall)		{ HRESULT _hres = (hrcall); if (FAILED(_hres)) { throw cs::ExceptionWindows(__FILE__, __FUNCTION__, __LINE__, _hres); } }
#ifdef CHSL_SAFE
#define EXC_COMCHECKINFO(hrcall)	{ cs::dxgiInfo::set(); HRESULT _hres = (hrcall); if (FAILED(_hres)) { throw cs::ExceptionWindows(__FILE__, __FUNCTION__, __LINE__, _hres, cs::dxgiInfo::getMessages()); } }
#define EXC_COMINFO(call)			{ cs::dxgiInfo::set(); (call); std::vector<std::string> v = cs::dxgiInfo::getMessages(); if (v.size() > 0) { throw cs::ExceptionGeneral(__FILE__, __FUNCTION__, __LINE__, cs::ExceptionWindows::TranslateMessageArray(cs::dxgiInfo::getMessages())); } }
#else
#define EXC_COMCHECKINFO(hrcall)	{ HRESULT _hres = (hrcall); if (FAILED(_hres)) { throw cs::ExceptionWindows(__FILE__, __FUNCTION__, __LINE__, _hres); } }
#define EXC_COMINFO(call)			{ (call); }
#endif
#endif

#endif
	
