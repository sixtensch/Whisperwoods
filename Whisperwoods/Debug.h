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
enum FMOD_RESULT;
class Sound;

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

	void CalculateFps(float dTime);

	void CaptureStreams(bool cout = true, bool cerr = true, bool clog = true);
	void CaptureSound(const Sound* sound);

	void DumpDevice();
	void LoadDeviceRef(ComPtr<ID3D11Device> device);

	static void PushMessage(string origin, DebugLevel level, const char* format, ...);
	static void PushMessage(string origin, const char* message, DebugLevel level = DebugLevelDebug);
	static void PushMessage(cstr file, cstr func, int line, DebugLevel level, const char* format, ...);
	static void PushMessage(cstr file, cstr func, int line, const char* message, DebugLevel level = DebugLevelDebug);

	static void ExecuteCommand(string origin, const char* command);
	static void ExecuteCommand(cstr file, cstr func, int line, const char* command);
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

	static const string& LevelToString(DebugLevel level);

	static Debug& Get();



private:

	// Internal data types

	class DebugStreambuf : public std::streambuf
	{
	public:
		DebugStreambuf();
		~DebugStreambuf() = default;

		void SetLevel(DebugLevel level);
		void SetOriginString(string originString);

	private:
		void Push();

		//int xsputn(char* c, long long s) override;
		int overflow(int c) override;
		int sync() override;

	private:
		DebugLevel m_level;
		cs::List<char> m_buffer;
		string m_originString;
	};

	struct DebugItem
	{
		DebugLevel level;
		std::string text;
		std::string origin;
		std::chrono::steady_clock::time_point time;
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

	void PPushMessage(DebugLevel level, const char* format, va_list args, std::string origin);
	void PPushMessage(const char* message, DebugLevel level, std::string origin);

	void CaptureStream(std::ios* stream, DebugLevel level, string originString);
	void ReleaseStreams();

	void TryCommand();
	void RegisterDefaultCommands();

	static void TryPrintDescription(const CommandItem& command, DebugLevel level);
	static void CommandHelp(void* params, void* userData);
	static void CommandLog(void* params, void* userData);

	static FMOD_RESULT LogFMOD(unsigned int flags, const char* file, int line, const char* func, const char* message);



	// Variables

	static Debug* s_debug;

	static constexpr int c_tempBufferSize = 2048;
	static constexpr int c_inputBufferSize = 128;
	static constexpr int c_dataBufferSize = 144;
	static constexpr int c_debutOutputTimeout = 100;
	static constexpr char c_commandChar = '>';

	// std::mutex* m_mutex;

	std::chrono::steady_clock::time_point m_startTime;

	DebugLevel m_debugLevel;
	DebugLevel m_displayLevel;
	bool m_initialized;
	string m_levelStrings[DebugLevelCount];

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

	// -----------------------------  fps stuff
	float dTimeAverage16;
	float dTimeAverage256;
	float fpsAverage16;
	float fpsAverage256;

	float dTimeAccumulator16;
	float dTimeAccumulator256;
	cs::Queue<float> dTimeQueue16;
	cs::Queue<float> dTimeQueue256;
	//-------------------------------

	ComPtr<ID3D11Device> m_deviceRef;

	ComPtr<IDXGIInfoQueue> m_dxgiInfoQueue;
	uint64 m_dxgiNext = 0u;
};



// ---------------------------- MACROS

#define LOG_FRAMETRACE(str, ...)	Debug::PushMessage(__FILE__, __FUNCTION__, __LINE__, DebugLevelFrameTrace, (str), __VA_ARGS__);
#define LOG_TRACE(str, ...)			Debug::PushMessage(__FILE__, __FUNCTION__, __LINE__, DebugLevelTrace, (str), __VA_ARGS__);
#define LOG(str, ...)				Debug::PushMessage(__FILE__, __FUNCTION__, __LINE__, DebugLevelDebug, (str), __VA_ARGS__);
#define LOG_WARN(str, ...)			Debug::PushMessage(__FILE__, __FUNCTION__, __LINE__, DebugLevelWarn, (str), __VA_ARGS__);
#define LOG_ERROR(str, ...)			Debug::PushMessage(__FILE__, __FUNCTION__, __LINE__, DebugLevelError, (str), __VA_ARGS__);
#define LOG_CRITICAL(str, ...)		Debug::PushMessage(__FILE__, __FUNCTION__, __LINE__, DebugLevelCritical, (str), __VA_ARGS__);
#define LOG_COMMAND(cmd)			Debug::ExecuteCommand(__FILE__, __FUNCTION__, __LINE__, (cmd));

#define EXC(str, ...)				throw cs::ExceptionGeneral(__FILE__, __FUNCTION__, __LINE__, (str), __VA_ARGS__);
#define EXC_TEMP()					throw cs::Exception(__FILE__, __FUNCTION__, __LINE__);
#define EXC_HR(hrcall)				{ HRESULT _hres = (hrcall); if (_hres != 0) { throw cs::ExceptionWindows(__FILE__, __FUNCTION__, __LINE__, _hres); } }
#define EXC_HRLAST()				{ HRESULT _hres = GetLastError(); if (_hres != 0) { throw cs::ExceptionWindows(__FILE__, __FUNCTION__, __LINE__, _hres); } }

#define EXC_COM(comcall)			{ HRESULT _hres = (comcall); if (FAILED(_hres)) { throw cs::ExceptionWindows(__FILE__, __FUNCTION__, __LINE__, _hres); } }
#ifdef WW_NODXGI
#define EXC_COMCHECK(comcall)		{ HRESULT _hres = (comcall); if (_hres != 0) { throw cs::ExceptionWindows(__FILE__, __FUNCTION__, __LINE__, _hres); } }
#define EXC_COMINFO(call)			{ (call); }
#else
#define EXC_COMCHECK(comcall)		{ string _message; Debug::DXGISet(); HRESULT _hres = (comcall); if (Debug::DXGIGet(_message) || FAILED(_hres)) { throw cs::ExceptionWindows(__FILE__, __FUNCTION__, __LINE__, _hres, _message); } }
#define EXC_COMINFO(call)			{ string _message; Debug::DXGISet(); (call); if (Debug::DXGIGet(_message)) { throw cs::ExceptionGeneral(__FILE__, __FUNCTION__, __LINE__, _message); } }
#endif
