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
	static Debug* s_singleton;
};

