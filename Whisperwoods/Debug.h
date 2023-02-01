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

class Debug sealed
{
public:
	Debug();
	~Debug();

	void Init(bool captureStreams);
	void DeInit(bool writeLog = false);

	void CreateConsole();
	void DestroyConsole();
	void DrawConsole();

	void ClearFrameTrace();

	void WriteHelp();
	void WriteLog();

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

	static Debug& Get();

	



private:

	// Internal data types

	class DebugStreambuf : public std::streambuf
	{
	public:
		DebugStreambuf();
		~DebugStreambuf();

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

	void PPushMessage(DebugLevel level, const char* format, va_list args);
	void PPushMessage(const char* message, DebugLevel level);

	void CaptureDebugOutput();

	void CaptureStream(std::ios* stream, DebugLevel level);
	void ReleaseStreams();

	void TryCommand();
	void RegisterDefaultCommands();

	static void TryPrintDescription(const CommandItem& command, DebugLevel level);
	static void CommandHelp(void* params, void* userData);
	static void CommandLog(void* params, void* userData);



	// Variables

	static Debug* s_singleton;

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
};

