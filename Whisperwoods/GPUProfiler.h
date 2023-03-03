#pragma once

#include <unordered_map>


class GPUProfiler {

	struct TimeBuffer {
		const uint buffSize = 256;
		cs::Queue<float> values = {};

		float timeSum = 0.0f;
		float lastAverage = 0.0f;

		void UpdateBuffer(float value) {
			timeSum += value;
			values.Push(value);
			if (values.Size() == buffSize)
			{
				timeSum -= values.Pop();
			}

			lastAverage = timeSum / values.Size();
		}
	};

	struct ProfileData {
		bool isQuerying = false;

		float minDelta = FLT_MAX;
		float maxDelta = 0.0f;

		TimeBuffer lastDiffs;

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

	void FinilizeAndPresent();

private:

	void PostEndFrameSummary();
	void DrawImGui();

	bool inline IsAllowedToUpdate();
	void InitProfileData(const std::string profileName);
	
private:
	std::unordered_map<std::string, ProfileData> m_profiles;

	// References to render core's device and context.
	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;

	uint m_waitFramesPerUpdate;
	uint m_frameCounter;
	bool m_isOn;
	bool m_writeSummaryToConsole;
	std::string m_lastSummary;

	// TODO: Maybe add struct that contains info gained after a frame summary. Can be used for posting in ImGui.
};