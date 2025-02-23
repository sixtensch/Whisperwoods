#include "core.h"
#include "GPUProfiler.h"
#include "imgui.h"

constexpr uint DEFAULT_PRESENT_FRAME_WAIT = 50u;

GPUProfiler::GPUProfiler(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, uint waitFramesPerUpdate) : 
	m_profiles({}), 
	m_device(device),
	m_context(context), 
	m_waitFramesPerUpdate(waitFramesPerUpdate), 
	m_frameCounter(0u), 
	m_isOn(false),
	m_writeSummaryToConsole(false),
	m_lastSummary("") {}

GPUProfiler::GPUProfiler(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: GPUProfiler(device, context, DEFAULT_PRESENT_FRAME_WAIT) {}
	
GPUProfiler::GPUProfiler()
	: GPUProfiler(nullptr, nullptr) {}


void GPUProfiler::TimestampBegin(const std::string profileName)
{
	if (m_profiles.find(profileName) == m_profiles.end())
	{
		InitProfileData(profileName);
	}

	if (!IsAllowedToUpdate())
		return;

	ProfileData& profile = m_profiles[profileName];

	if (!profile.isQuerying)
	{
		profile.isQuerying = true;

		m_context->Begin(profile.queryDisjoint.Get());
		m_context->End(profile.queryTimeStart.Get());
	}
}

void GPUProfiler::TimestampEnd(const std::string profileName)
{
	if (!IsAllowedToUpdate())
		return;

	ProfileMapIterator profileIt = m_profiles.find(profileName);
	if (profileIt != m_profiles.end())
	{
		ProfileData& profile = profileIt->second;
		
		if (profile.isQuerying)
		{
			profile.isQuerying = false;
			m_context->End(profile.queryTimeEnd.Get());
			m_context->End(profile.queryDisjoint.Get());
		}
	}
}

void GPUProfiler::FinilizeAndPresent()
{
	if (m_isOn) 
	{
		SynthesizeSummary();
		m_frameCounter++;
	}

	DrawImGui();
}

void GPUProfiler::InitProfileData(const std::string profileName)
{
	ProfileData profile = {};

	D3D11_QUERY_DESC qdesc = {};
	qdesc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
	EXC_COMCHECK(m_device->CreateQuery(&qdesc, profile.queryDisjoint.GetAddressOf()));

	qdesc.Query = D3D11_QUERY_TIMESTAMP;
	EXC_COMCHECK(m_device->CreateQuery(&qdesc, profile.queryTimeStart.GetAddressOf()));
	EXC_COMCHECK(m_device->CreateQuery(&qdesc, profile.queryTimeEnd.GetAddressOf()));

	// Wont emplace if it already exits (this should never happen if used correctly).
	m_profiles.try_emplace(profileName, std::move(profile));
	m_insertionOrder.Add(profileName);
}

void GPUProfiler::SynthesizeSummary()
{
	if (!IsAllowedToUpdate())
		return;

	static const char profileFormat[] = "[%s]\nDiff %.5f | Avg %.5f | Max %.5f | Min %.5f\n\n";
	float summedTime = 0.0f;

	std::string frameSummary = "\n\n-- GPU PROFILE SUMMARY (in ms) --\n\n";
	for (const std::string& profileName : m_insertionOrder)
	{
		ProfileData& profile = m_profiles[profileName];

		UINT64 startStamp = 0u;
		while (m_context->GetData(profile.queryTimeStart.Get(), (void*)&startStamp, sizeof(startStamp), 0u) != S_OK);

		UINT64 endStamp = 0u;
		while (m_context->GetData(profile.queryTimeEnd.Get(), (void*)&endStamp, sizeof(endStamp), 0u) != S_OK);

		D3D11_QUERY_DATA_TIMESTAMP_DISJOINT queryData = {};
		while (m_context->GetData(profile.queryDisjoint.Get(), (void*)&queryData, sizeof(queryData), 0u) != S_OK);


		if (!queryData.Disjoint)
		{
			const size_t maxBufSize = 256;
			char profileTextBuff[maxBufSize] = {0};

			UINT64 stampDiff = endStamp - startStamp;
			// Time in milliseconds.
			float timeDiff = (stampDiff / (float)queryData.Frequency) * 1000.0f;
			summedTime += timeDiff;

			profile.lastDiffs.UpdateBuffer(timeDiff);

			float currentMax = profile.maxDelta;
			float currentMin = profile.minDelta;
			profile.maxDelta = currentMax < timeDiff ? timeDiff : currentMax;
			profile.minDelta = currentMin > timeDiff ? timeDiff : currentMin;

			std::snprintf(profileTextBuff, maxBufSize - 1, profileFormat, 
				profileName.c_str(), 
				timeDiff, 
				profile.lastDiffs.lastAverage,
				profile.maxDelta, 
				profile.minDelta
			);

			frameSummary += std::string(profileTextBuff);
		}
		else
		{
			LOG_WARN("Query data for [%s] is disjoint. Timing is invalid.", profileName.c_str());
		}
	}

	frameSummary.append("\nTotal time sum (ms): ").append(std::to_string(summedTime));

	frameSummary += "\n-------- SUMMARY END --------\n";

	if (m_writeSummaryToConsole)
	{
		LOG(frameSummary.c_str());
	}
	
	m_lastSummary = std::move(frameSummary);
}

void GPUProfiler::DrawImGui()
{
#if WW_IMGUI 1
	if (ImGui::Begin("GPU Profiler"))
	{
		ImGui::Checkbox("Profiler toggle", &m_isOn);
		ImGui::Checkbox("Write duplicate summary to console", &m_writeSummaryToConsole);

		ImGui::Text("Frames per update (Lower values will tank FPS)");
		ImGui::InputInt(" ", (int*)&m_waitFramesPerUpdate, 1, 10);

		ImGui::Text(m_lastSummary.c_str());
	}
	ImGui::End();

	// TODO: Simple fix for avoiding user setting negative value of update frequency. Find a better ImGui solution for this?
	static uint s_maxFrameWait = 10000u;
	static uint s_minFrameWait = 10u;
	if (m_waitFramesPerUpdate < s_minFrameWait)
	{
		m_waitFramesPerUpdate = s_minFrameWait;
	}
	else if (m_waitFramesPerUpdate > s_maxFrameWait)
	{
		m_waitFramesPerUpdate = s_maxFrameWait;
	}
#endif
}

bool GPUProfiler::IsAllowedToUpdate()
{
	return ((m_frameCounter % m_waitFramesPerUpdate) == 0) && m_isOn;
}
