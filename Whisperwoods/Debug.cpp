#include "core.h"
#include "Debug.h"
#include "Sound.h"

#include <dxgidebug.h>

#include <fmod.h>

#include <CHSL/Debug.h>
#include <unordered_map>

#include <imgui.h>

#include <fcntl.h>
#include <io.h>
#include <format>



Debug* Debug::s_debug = nullptr;

#pragma comment(lib, "dxguid.lib")





Debug::Debug()
	:
	m_debugLevel(DebugLevelFrameTrace),
	m_displayLevel(DebugLevelDebug),
	m_initialized(false),
	m_scrollToBottom(false),
	m_showCommandLine(true),

	m_tempBuffer(nullptr),
	m_inputBuffer(nullptr),

	

	m_textColors
	{
		{ 0.4f, 0.4f, 0.4f, 1.0f },
		{ 0.6f, 0.6f, 0.6f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 0.4f, 1.0f },
		{ 1.0f, 0.6f, 0.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f, 1.0f },

		{ 0.2f, 0.5f, 0.2f, 1.0f },
		{ 0.4f, 1.0f, 0.4f, 1.0f },

		{}
	},

	m_levelStrings
	{
		"Frame Trace",
		"Trace",
		"Debug",
		"Warning",
		"Error",
		"Critical Error",
		"Command Trace",
		"Command",
		"None"
	}
{
	if (s_debug != nullptr)
	{
		throw "Debug singleton re-initialization.";	// TODO: Proper exceptions
	}

	s_debug = this;

	dTimeAverage16 = 0.0f;
	dTimeAverage256 = 0.0f;
	fpsAverage16 = 0.0f;
	fpsAverage256 = 0.0f;
	dTimeAccumulator16 = 0.0f;
	dTimeAccumulator256 = 0.0f;

	m_initialized = true;
	m_tempBuffer = new char[c_tempBufferSize + 1] { '\0' };
	m_inputBuffer = new char[c_inputBufferSize + 1] { '\0' };
	m_dataBuffer = new byte[c_dataBufferSize + 1]{ 0 };

	m_commands = new std::unordered_map<std::string, CommandItem>();

	m_startTime = std::chrono::steady_clock::now();

	RegisterDefaultCommands();

	InitDXGI();
}

Debug::~Debug()
{
	s_debug = nullptr;

	m_initialized = false;
	delete[] m_tempBuffer;
	delete[] m_inputBuffer;
	delete[] m_dataBuffer;

	delete (std::unordered_map<std::string, CommandItem>*)m_commands;

	ReleaseStreams();

	DeInitDGXI();
}

void Debug::CreateConsole()
{
}

void Debug::DestroyConsole()
{
}

void Debug::DrawConsole()
{
	if (ImGui::Begin("Debug Console", nullptr, ImGuiWindowFlags_MenuBar))
	{
		

	
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Config"))
			{
				static const char* strings[7] = { "Frame Trace", "Trace", "Debug", "Warn", "Error", "Critical", "Commands" };
				if (ImGui::ListBox(
					"Debug Level",
					(int*)&m_debugLevel,
					strings,
					7))
				{
					PushMessage("Triggered from console widget.", (DebugLevel)std::max(m_debugLevel, DebugLevelWarn), "Debug level set to %s.", strings[m_debugLevel]);
				}
				ImGui::EndMenu();
			}
			if (ImGui::Begin("FPS counter"))
			{
				ImGui::Text("Last 16 frames average FPS: %f", fpsAverage16);
				ImGui::Text("Last 256 frames average FPS: %f", fpsAverage256);
				ImGui::End();
			}
			if (ImGui::BeginMenu("Display"))
			{
				static const char* strings[7] = { "Frame Trace (!)", "Trace", "Debug", "Warn", "Error", "Critical", "Commands" };
				ImGui::ListBox(
					"Filter",
					(int*)&m_displayLevel,
					strings,
					7
				);
				ImGui::Separator();
				ImGui::Checkbox("Command Line", &m_showCommandLine);
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}


		float consoleHeight = 0.0f;
		if (m_showCommandLine)
		{
			consoleHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		}

		if (ImGui::BeginChild("Console", ImVec2(0, -consoleHeight)))
		{
			ImGui::PushTextWrapPos();

			for (int i = 0; i < m_items.Size(); i++)
			{
				DebugItem& item = m_items[i];

				if (item.level < m_displayLevel)
				{
					continue;
				}

				if (item.level != DebugLevelCommandTrace)
				{
					ImGui::Spacing();
				}

				ImGui::PushStyleColor(ImGuiCol_Text, *(ImVec4*)&m_textColors[item.level]);
				ImGui::TextUnformatted(item.text.data());
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
				{
					using namespace std::chrono;

					ImGui::SetTooltip("%s\n\nLevel: %s\n\nTime:\n%s\n%s",
						item.origin.c_str(),
						LevelToString(item.level).c_str(),
						std::format("{:%T} since startup", floor<microseconds>(item.time - m_startTime)).c_str(),
						std::format("{:%T} ago", floor<milliseconds>(steady_clock::now() - item.time)).c_str()
					);
				}
				ImGui::PopStyleColor();
			}

			ImGui::PopTextWrapPos();

			if (m_scrollToBottom)
			{
				ImGui::SetScrollHereY(1.0f);
				m_scrollToBottom = false;
			}

			ImGui::Spacing();

			ImGui::EndChild();
		}

		if (m_showCommandLine)
		{
			ImGui::Separator();

			bool buttonPress = ImGui::Button("Execute");
			ImGui::SameLine();
			ImGui::PushItemWidth(-1);
			bool enterPress = ImGui::InputText("Input", m_inputBuffer, c_inputBufferSize, ImGuiInputTextFlags_EnterReturnsTrue);

			if ((buttonPress || enterPress))
			{
				if (m_inputBuffer[0] != '\0')
				{
					PushMessage("Triggered from console widget.", DebugLevelCommand, "%c %s", c_commandChar, m_inputBuffer);
					m_scrollToBottom = true;
				}

				std::memset(m_inputBuffer, '\0', std::strlen(m_inputBuffer));
			}
			ImGui::PopItemWidth();

			ImGui::SetItemDefaultFocus();
			if (buttonPress || enterPress)
			{
				ImGui::SetKeyboardFocusHere(-1);
			}
		}
	}
	ImGui::End();
}

void Debug::ClearFrameTrace()
{
	if (m_frameTraceIndices.Size() > 0)
	{
		m_items.MassRemove(m_frameTraceIndices.Data(), m_frameTraceIndices.Size());
		m_frameTraceIndices.Clear(false);
	}
}

void Debug::WriteHelp()
{
	for (std::pair<std::string, CommandItem> p : *((std::unordered_map<std::string, CommandItem>*)m_commands))
	{
		TryPrintDescription(p.second, DebugLevelCommandTrace);
	}
}

void Debug::WriteLog()
{
	// TODO(sixten): Write logs.

	LOG_ERROR("Log file writing currently not implemented.");
}

void Debug::CalculateFps(float dTime) // Calculates fps to member variables Imgui uses
{
	dTimeAccumulator16 += dTime;
	dTimeAccumulator256 += dTime;
	dTimeQueue16.Push(dTime);
	dTimeQueue256.Push(dTime);

	if (dTimeQueue16.Size() == 16)
	{
		dTimeAccumulator16 -= dTimeQueue16.Pop();
	}

	if (dTimeQueue256.Size() == 256)
	{
		dTimeAccumulator256 -= dTimeQueue256.Pop();
	}

	dTimeAverage16 = dTimeAccumulator16 / dTimeQueue16.Size();
	dTimeAverage256 = dTimeAccumulator256 / dTimeQueue256.Size();
	fpsAverage16 = 1.0f / dTimeAverage16;
	fpsAverage256 = 1.0f / dTimeAverage256;
}



Debug& Debug::Get()
{
#ifdef WW_DEBUG
	if (s_debug == nullptr)
	{
		throw "Debug singleton not found.";	// TODO: Proper exceptions
	}
#endif

	return *s_debug;
}

void Debug::CaptureStreams(bool cout, bool cerr, bool clog)
{
	if (cout)
	{
		CaptureStream(&std::cout, DebugLevelDebug, "From std::cout stream");
	}

	if (cerr)
	{
		CaptureStream(&std::cerr, DebugLevelDebug, "From std::cerr stream");
	}

	if (clog)
	{
		CaptureStream(&std::clog, DebugLevelDebug, "From std::clog stream");
	}
}

void Debug::CaptureSound(const Sound* sound)
{
	FMOD::Debug_Initialize(FMOD_DEBUG_LEVEL_LOG, FMOD_DEBUG_MODE_CALLBACK, LogFMOD);
}

void Debug::DumpDevice()
{
	ComPtr<ID3D11Debug> d3d11Debug;
	m_deviceRef->QueryInterface(__uuidof(ID3D11Debug), (void**)(&d3d11Debug));
	d3d11Debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
}

void Debug::LoadDeviceRef(ComPtr<ID3D11Device> device)
{
	m_deviceRef = device;
}

void Debug::PushMessage(string origin, DebugLevel level, const char* format, ...)
{
	if (!s_debug || s_debug->m_debugLevel > level)
	{
		return;
	}

	va_list args;
	va_start(args, format);
	s_debug->PPushMessage(level, format, args, origin);
	va_end(args);
}

void Debug::PushMessage(string origin, const char* message, DebugLevel level)
{
	if (!s_debug || s_debug->m_debugLevel > level)
	{
		return;
	}

	s_debug->PPushMessage(message, level, origin);
}

void Debug::PushMessage(cstr file, cstr func, int line, DebugLevel level, const char* format, ...)
{
	if (!s_debug || s_debug->m_debugLevel > level)
	{
		return;
	}

	std::string origin = std::string("File: ") + file + "\nFunction: " + func + "\nLine: " + std::to_string(line);

	va_list args;
	va_start(args, format);
	s_debug->PPushMessage(level, format, args, origin);
	va_end(args);
}

void Debug::PushMessage(cstr file, cstr func, int line, const char* message, DebugLevel level)
{
	std::string origin = std::string("File: ") + file + "\nFunction: " + func + "\nLine: " + std::to_string(line);

	PushMessage(origin, message, level);
}

void Debug::ExecuteCommand(string origin, const char* command)
{
	if (command[0] == c_commandChar)
	{
		PushMessage(origin, command, DebugLevelCommand);
	}
	else
	{
		PushMessage(origin, DebugLevelCommand, "%c %s", c_commandChar, command);
	}
}

void Debug::ExecuteCommand(cstr file, cstr func, int line, const char* command)
{
	if (command[0] == c_commandChar)
	{
		PushMessage(file, func, line, command, DebugLevelCommand);
	}
	else
	{
		PushMessage(file, func, line, DebugLevelCommand, "%c %s", c_commandChar, command);
	}
}

void Debug::RegisterCommand(DebugCommandCallback callback, const DebugCommandParamType parameters[4], const std::string& identifier, const std::string& description, void* userData)
{
	if (!s_debug)
	{
		return;
	}

	int paramCount = 0;
	for (; paramCount < 4 && parameters[paramCount] != DebugCommandParamTypeNone; paramCount++);

	(*((std::unordered_map<std::string, CommandItem>*)s_debug->m_commands))[identifier] =
	{
		identifier,
		description,
		callback,
		{ parameters[0], parameters[1], parameters[2], parameters[3] },
		paramCount,
		userData
	};
}

void Debug::RegisterCommand(DebugCommandCallback callback, const std::string& identifier, const std::string& description, void* userData)
{
	DebugCommandParamType noParams[4] = { DebugCommandParamTypeNone };
	RegisterCommand(callback, noParams, identifier, description, userData);
}



void Debug::DXGISet()
{
	s_debug->m_dxgiNext = s_debug->m_dxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

bool Debug::DXGIGet(std::string& out)
{
	uint64 end = s_debug->m_dxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);

	if (s_debug->m_dxgiNext == end)
	{
		return false;
	}

	out = "\n";

	for (auto i = s_debug->m_dxgiNext; i < end; ++i)
	{
		uint64 messageLength = 0;

		DXGI_INFO_QUEUE_MESSAGE msg;

		// get the size of message i in bytes
		s_debug->m_dxgiInfoQueue->GetMessageA(DXGI_DEBUG_ALL, i, &msg, &messageLength);

		// allocate memory for message
		auto bytes = std::make_unique<byte[]>(messageLength);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());

		// get the message and push its description into the vector
		s_debug->m_dxgiInfoQueue->GetMessageA(DXGI_DEBUG_ALL, i, pMessage, &messageLength);

		out += pMessage->pDescription;
		out += "\n";

		static char buf[512] = { '\0' };
	}

	return true;
}

const string& Debug::LevelToString(DebugLevel level)
{
	return s_debug->m_levelStrings[level];
}



// Private functions

void Debug::InitDXGI()
{
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

	// load the dll that contains the function DXGIGetDebugInterface
	const auto hModDxgiDebug = LoadLibraryExA("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (hModDxgiDebug == nullptr)
	{
		EXC_HRLAST();
	}

	// get address of DXGIGetDebugInterface in dll
	const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
		reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface")));

	if (DxgiGetDebugInterface == nullptr)
	{
		EXC_HRLAST();
	}

	DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &m_dxgiInfoQueue);
}

void Debug::DeInitDGXI()
{
	m_dxgiInfoQueue.Reset();
}

int ImFormatStringV(char* buf, size_t buf_size, const char* fmt, va_list args);

void Debug::PPushMessage(DebugLevel level, const char* format, va_list args, string origin)
{
	const char* end = m_tempBuffer + ImFormatStringV(m_tempBuffer, c_tempBufferSize, format, args);

	// m_mutex->lock();

	m_items.Add(DebugItem{ level, std::string(m_tempBuffer, (int)(end - m_tempBuffer)), origin, std::chrono::steady_clock::now() });

	if (level == DebugLevelFrameTrace)
	{
		m_frameTraceIndices.Add(m_items.Size() - 1);
	}
	else
	{
		//std::cout << m_items.Back().text.c_str() << std::endl;
		OutputDebugStringA((m_items.Back().text + '\n').c_str());
	}

	TryCommand();

	// m_mutex->unlock();
}

void Debug::PPushMessage(const char* message, DebugLevel level, string origin)
{
	//m_mutex->lock();

	m_items.Add(DebugItem{ level, std::string(message), origin, std::chrono::steady_clock::now() });

	if (level == DebugLevelFrameTrace)
	{
		m_frameTraceIndices.Add(m_items.Size() - 1);
	}
	else
	{
		//std::cout << m_items.Back().text.c_str() << std::endl;
		OutputDebugStringA((m_items.Back().text + '\n').c_str());
	}

	TryCommand();

	//m_mutex->unlock();
}

void Debug::CaptureStream(std::ios* stream, DebugLevel level, string originString)
{
	for (int i = 0; i < m_streams.Size(); i++)
	{
		if (m_streams[i].stream == stream)
		{
			LOG_WARN("Tried to capture already registered output stream.");
			return;
		}
	}

	m_streams.Add({ {}, stream->rdbuf(), stream});

	StreamRedirect& d = m_streams.Back();
	d.buffer.SetLevel(level);
	d.buffer.SetOriginString(originString);
	stream->rdbuf(&d.buffer);
}

void Debug::ReleaseStreams()
{
	for (StreamRedirect& s : m_streams)
	{
		s.stream->rdbuf(s.streamBuffer);
	}

	m_streams.Clear();
}

void Debug::TryCommand()
{
	const DebugItem& item = m_items.Back();

	if (item.text[0] == c_commandChar)
	{
		std::stringstream full(&item.text[1]);
		std::string id;
		full >> id;

		std::unordered_map<std::string, CommandItem>* c = (std::unordered_map<std::string, CommandItem>*)m_commands;

		if (c->contains(id))
		{
			int paramsCount = 0;
			std::string params[4];
			while (!full.eof())
			{
				full >> params[paramsCount];
				paramsCount++;
			}

			const CommandItem& command = (*c)[id];

			if (paramsCount > command.parameterCount)
			{
				LOG_ERROR("Excess parameters given, command \"%s\" accepts a maximum of %i parameters.", id.c_str(), command.parameterCount);
				TryPrintDescription(command, DebugLevelWarn);
				return;
			}
			else if (paramsCount < command.parameterCount)
			{
				LOG_ERROR("Insufficient parameters given, command \"%s\" requires %i parameters.", id.c_str(), command.parameterCount);
				TryPrintDescription(command, DebugLevelError);
				return;
			}
			else
			{
				byte* current = m_dataBuffer;
				byte* end = m_dataBuffer + c_dataBufferSize - 1;

				try
				{
					for (int i = 0; i < paramsCount; i++)
					{
						switch (command.parameters[i])
						{
						case DebugCommandParamTypeInt:
						{
							int* p = (int*)current;
							*p = std::stoi(params[i]);
							current[sizeof(int)] = 0;
							current += sizeof(int);
						}
						break;

						case DebugCommandParamTypeFloat:
						{
							float* p = (float*)current;
							*p = std::stof(params[i]);
							current[sizeof(float)] = 0;
							current += sizeof(float);
						}
						break;

						case DebugCommandParamTypeString:
						{
							size_t size = params[i].size();
							end -= (size + 1);
							std::memcpy(end, params[i].data(), size);
							end[size] = 0;

							const char** p = (cstr*)current;
							*p = (cstr)end;
							current += sizeof(cstr);
						}
						break;

						default:	// Includes TypeNone
							break;
						}
					}
				}
				catch (...)
				{
					LOG_ERROR("Error parsing parameters.", id.c_str(), command.parameterCount);
					TryPrintDescription(command, DebugLevelError);
					return;
				}

				*current = 0;
			}

			// Redirect to the callback function.
			command.callback(m_dataBuffer, command.userData);
		}
		else
		{
			LOG_WARN("Command \"%s\" not recognized. Type \"help\" for a list of registered commands.", id.c_str(), c_commandChar);
		}
	}
}

void Debug::RegisterDefaultCommands()
{
	RegisterCommand(CommandHelp, "help", "Display all registered commands.", this);
	RegisterCommand(CommandLog, "log", "Log debug stream to file.", this);
}

void Debug::TryPrintDescription(const CommandItem& command, DebugLevel level)
{
	static std::string typeStrings[DebygCommandParamTypeCount] =
	{
		"None",
		"Int",
		"Float",
		"String"
	};

	std::string message = command.identifier;

	if (command.parameterCount > 0)
	{
		message += " [";

		for (int i = 0; i < command.parameterCount; i++)
		{
			if (i > 0)
			{
				message += ", ";
			}

			message += typeStrings[command.parameters[i]].c_str();
		}

		message += "]";
	}

	if (command.description != "")
	{
		message += ": \"" + command.description + "\"";
	}

	PushMessage("Triggered from help command.", message.c_str(), DebugLevelCommandTrace);
}

void Debug::CommandHelp(void* params, void* userData)
{
	Debug* d = (Debug*)userData;
	d->WriteHelp();
}

void Debug::CommandLog(void* params, void* userData)
{
	Debug* d = (Debug*)userData;
	d->WriteLog();
}

FMOD_RESULT Debug::LogFMOD(unsigned int flags, const char* file, int line, const char* func, const char* message)
{
	PushMessage("Triggered by internal FMOD system.", message, DebugLevelTrace);
	return FMOD_OK;
}






// ----------------------------------------- DebugStreambuf

Debug::DebugStreambuf::DebugStreambuf()
	:
	std::streambuf(),
	m_level(DebugLevelDebug)
{
}

void Debug::DebugStreambuf::SetLevel(DebugLevel level)
{
	m_level = level;
}

void Debug::DebugStreambuf::SetOriginString(string originString)
{
	m_originString = originString;
}

void Debug::DebugStreambuf::Push()
{
	m_buffer.Add('\0');
	PushMessage(m_originString, m_buffer.Data(), m_level);
	m_buffer.Clear(false);
}

int Debug::DebugStreambuf::overflow(int c)
{
	int result = EOF;

	if (c < 0 || c == '\n' || c == '\r')
	{
		result = sync();
		Push();
	}
	else if (c >= 0 && c <= UCHAR_MAX)
	{
		result = c;
		m_buffer.Add(c);
	}
	else
	{
		result = c;
		LOG_WARN("Invalid value [%x] pushed to DebugStreambuf.", c);
	}

	return result;
}

int Debug::DebugStreambuf::sync()
{
	return 0;
}
