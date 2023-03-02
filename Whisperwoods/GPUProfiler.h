#pragma once

#include <unordered_map>

#define USE_GPU_PROFILER false

class GPUProfiler {

	struct ProfileData {
		bool isQuerying = false;

		ComPtr<ID3D11Query> queryDisjoint = nullptr;
		ComPtr<ID3D11Query> queryTimeStart = nullptr;
		ComPtr<ID3D11Query> queryTimeEnd = nullptr;
	};

	typedef std::unordered_map<std::string, ProfileData>::iterator ProfileMapIterator;

public:

	GPUProfiler();
	GPUProfiler(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
	GPUProfiler(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, uint updateFrequency);

	void TimestampBegin(const std::string profileName);
	void TimestampEnd(const std::string profileName);

	void PostEndFrameSummary(bool useImGui = false);

private:

	bool inline IsAllowedToUpdate();
	void InitProfileData(const std::string profileName);
	
private:
	std::unordered_map<std::string, ProfileData> m_profiles;

	// References to render core's device and context.
	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;

	uint m_presentFrequency;
	uint m_presentCounter;
};