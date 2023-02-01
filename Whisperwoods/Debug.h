#pragma once

// Whisperwoods debug framework
// See debug levels below



enum DebugLevel
{
	// Most verbose. Information sometimes useful, and additionally expected to spam the console in real time.
	DebugLevelFrameTrace,	
	
	// Information sometimes useful on debug, usually as subtext to other debug logs.
	DebugLevelTrace,		
	
	// Information judged to be useful on debug.
	DebugLevelDebug,		
	
	// Information related to potentially unintended behavior.
	DebugLevelWarn,			
	
	// Information related to erroneous behavior.
	DebugLevelError,

	// Least verbose. Information related to critical errors and system-breaking behavior likely to cause further errors.
	DebugLevelCritical,		



	DebugLevelCommandTrace,	// Information following from commands.
	DebugLevelCommand,		// Information specifically triggered by user commands for debugging.

	DebugLevelNone,			// No output.

	DebugLevelCount		// <-- Keep last!
};

enum DebugCommandParamType
{
	DebugCommandParamTypeNone,
	DebugCommandParamTypeInt,		// int
	DebugCommandParamTypeFloat,		// float
	DebugCommandParamTypeString,	// const char*, null-terminated

	DebygCommandParamTypeCount		// <-- Keep last!
};

typedef void (*DebugCommandCallback)(void* params, void* userData);

struct IDXGIInfoQueue;

class Debug sealed
{
public:
	Debug();
	~Debug();

	void CreateConsole();
	void DestroyConsole();
	void DrawConsole();

	void ClearFrameTrace();

	void WriteHelp();
	void WriteLog();

	void CaptureStreams(bool cout = true, bool cerr = true, bool clog = true);

	void DumpDevice();
	void LoadDeviceRef(ComPtr<ID3D11Device> device);

	static void PushMessage(DebugLevel level, const char* format, ...);
	static void PushMessage(const char* message, DebugLevel level = DebugLevelDebug);

	static void ExecuteCommand(const char* command);
	static void RegisterCommand(
		DebugCommandCallback callback,
		const DebugCommandParamType parameters[4],
		const std::string& identifier,
		const std::string& description = "",
		void* userData = nullptr);
	static void RegisterCommand(
		DebugCommandCallback callback,
		const std::string& identifier,
		const std::string& description = "",
		void* userData = nullptr);

	static void DXGISet();
	static bool DXGIGet(std::string& out);

	static Debug& Get();

	



private:

	// Internal data types

	class DebugStreambuf : public std::streambuf
	{
	public:
		DebugStreambuf();
		~DebugStreambuf() = default;

		void SetLevel(DebugLevel level);

	private:
		void Push();

		//int xsputn(char* c, long long s) override;
		int overflow(int c) override;
		int sync() override;

	private:
		DebugLevel m_level;
		cs::List<char> m_buffer;
	};

	struct DebugItem
	{
		DebugLevel level;
		std::string text;
	};

	struct CommandItem
	{
		std::string identifier;
		std::string description;
		DebugCommandCallback callback;
		DebugCommandParamType parameters[4];
		int parameterCount;
		void* userData;
	};

	struct StreamRedirect
	{
		DebugStreambuf buffer;
		std::streambuf* streamBuffer;
		std::ios* stream;
	};



	// Member functions

	void InitDXGI();
	void DeInitDGXI();

	void PPushMessage(DebugLevel level, const char* format, va_list args);
	void PPushMessage(const char* message, DebugLevel level);

	void CaptureStream(std::ios* stream, DebugLevel level);
	void ReleaseStreams();

	void TryCommand();
	void RegisterDefaultCommands();

	static void TryPrintDescription(const CommandItem& command, DebugLevel level);
	static void CommandHelp(void* params, void* userData);
	static void CommandLog(void* params, void* userData);



	// Variables

	static Debug* s_debug;

	static constexpr int c_tempBufferSize = 2048;
	static constexpr int c_inputBufferSize = 128;
	static constexpr int c_dataBufferSize = 144;
	static constexpr int c_debutOutputTimeout = 100;
	static constexpr char c_commandChar = '>';

	std::mutex* m_mutex;

	DebugLevel m_debugLevel;
	DebugLevel m_displayLevel;
	bool m_initialized;

	cs::List<DebugItem> m_items;
	cs::List<StreamRedirect> m_streams;

	cs::Vec4 m_textColors[DebugLevelCount];

	void* m_commands;
	cs::List<int> m_frameTraceIndices;

	//Window* m_consoleWindow;
	//std::thread m_consoleThread;
	char* m_tempBuffer;
	char* m_inputBuffer;
	byte* m_dataBuffer;
	bool m_scrollToBottom;
	bool m_showCommandLine;

	ComPtr<ID3D11Device> m_deviceRef;

	ComPtr<IDXGIInfoQueue> m_dxgiInfoQueue;
	uint64 m_dxgiNext = 0u;
};



// ---------------------------- MACROS

#define LOG_FRAMETRACE(str, ...)	Debug::PushMessage(DebugLevelFrameTrace, (str), __VA_ARGS__);
#define LOG_TRACE(str, ...)			Debug::PushMessage(DebugLevelTrace, (str), __VA_ARGS__);
#define LOG(str, ...)				Debug::PushMessage(DebugLevelDebug, (str), __VA_ARGS__);
#define LOG_WARN(str, ...)			Debug::PushMessage(DebugLevelWarn, (str), __VA_ARGS__);
#define LOG_ERROR(str, ...)			Debug::PushMessage(DebugLevelError, (str), __VA_ARGS__);
#define LOG_CRITICAL(str, ...)		Debug::PushMessage(DebugLevelCritical, (str), __VA_ARGS__);
#define LOG_COMMAND(cmd)			Debug::ExecuteCommand((cmd));

#define EXC(str, ...)				throw cs::ExceptionGeneral(__FILE__, __FUNCTION__, __LINE__, (str), __VA_ARGS__);
#define EXC_TEMP()					throw cs::Exception(__FILE__, __FUNCTION__, __LINE__);
#define EXC_HR(hrcall)				{ HRESULT _hres = (hrcall); if (_hres != 0) { throw cs::ExceptionWindows(__FILE__, __FUNCTION__, __LINE__, _hres); } }
#define EXC_HRLAST()				{ HRESULT _hres = GetLastError(); if (_hres != 0) { throw cs::ExceptionWindows(__FILE__, __FUNCTION__, __LINE__, _hres); } }

#define EXC_COM(comcall)			{ HRESULT _hres = (hrcall); if (FAILED(_hres)) { throw cs::ExceptionWindows(__FILE__, __FUNCTION__, __LINE__, _hres); } }
#ifdef WW_NODXGI
#define EXC_COMCHECK(comcall)		{ HRESULT _hres = (hrcall); if (_hres != 0) { throw cs::ExceptionWindows(__FILE__, __FUNCTION__, __LINE__, _hres); } }
#define EXC_COMINFO(call)			{ (call); }
#else
#define EXC_COMCHECK(comcall)		{ string _message; Debug::DXGISet(); HRESULT _hres = (hrcall); if (FAILED(_hres) || Debug::DXGIGet(_message)) { throw cs::ExceptionWindows(__FILE__, __FUNCTION__, __LINE__, _hres, _message); } }
#define EXC_COMLAST(call)			{ string _message; Debug::DXGISet(); (call); if (Debug::DXGIGet(_message)) { throw cs::ExceptionGeneral(__FILE__, __FUNCTION__, __LINE__, _message); } }
#endif
