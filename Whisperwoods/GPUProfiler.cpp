#include "core.h"
#include "GPUProfiler.h"
#include "imgui.h"

constexpr uint DEFAULT_PRESENT_FREQ = 50u;

GPUProfiler::GPUProfiler(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, uint updateFrequency)
	: m_profiles({}), m_device(device), m_context(context), m_presentFrequency(updateFrequency), m_presentCounter(0u) {}

GPUProfiler::GPUProfiler(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: GPUProfiler(device, context, DEFAULT_PRESENT_FREQ) {}
	
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
}

void GPUProfiler::PostEndFrameSummary(bool useImGui /*= false*/)
{
	m_presentCounter++;

	if (useImGui)
	{
		//TODO: Add proper imgui support.

		if (ImGui::Begin("GPU Profiler"))
		{
			ImGui::Text("No idea what to put here :)");
		}
		ImGui::End();
	}

	if (!IsAllowedToUpdate())
		return;

	std::string frameSummary = "\n\n-- GPU PROFILE SUMMARY --\n\n";
	for (const auto& [profileName, profile] : m_profiles)
	{
		UINT64 startStamp = 0u;
		while (m_context->GetData(profile.queryTimeStart.Get(), (void*)&startStamp, sizeof(startStamp), 0u) != S_OK);

		UINT64 endStamp = 0u;
		while (m_context->GetData(profile.queryTimeEnd.Get(), (void*)&endStamp, sizeof(endStamp), 0u) != S_OK);

		D3D11_QUERY_DATA_TIMESTAMP_DISJOINT queryData = {};
		while (m_context->GetData(profile.queryDisjoint.Get(), (void*)&queryData, sizeof(queryData), 0u) != S_OK);


		if (!queryData.Disjoint)
		{
			UINT64 stampDiff = endStamp - startStamp;
			// Time in milliseconds.
			float timeDiff = (stampDiff / (float)queryData.Frequency) * 1000.0f;

			frameSummary.append(profileName).append(": ").append(std::to_string(timeDiff)).append(" ms\n");
		}
		else
		{
			LOG_WARN("Query data for '%s' is disjoint. Timing is invalid.", profileName.c_str());
		}
	}

	frameSummary += "\n-------- SUMMARY END --------\n";

	LOG(frameSummary.c_str());
}

bool GPUProfiler::IsAllowedToUpdate()
{
	return (m_presentCounter % m_presentFrequency) == 0;
}
