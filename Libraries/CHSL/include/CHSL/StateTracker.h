#pragma once

namespace cs
{

	class StateTracker
	{
	public:
		enum State
		{
			StateRunning,
			StateUnder,
			StateOver
		};

	public:
		StateTracker(float keys[], int keyCount, float start = 0.0f);
		StateTracker(const StateTracker& lVal);
		StateTracker(StateTracker&& rVal);
		StateTracker() = delete;
		virtual ~StateTracker();

		virtual StateTracker& operator=(const StateTracker& lVal);
		virtual StateTracker& operator=(StateTracker&& rVal);

		void Reset();
		void Lapse(float difference);
		void GoToTime(float time);
		void GoToInterval(int interval);
		void GoToInterval(int interval, float additionalTime);

		int GetInterval() const;
		float GetTime() const;
		float GetTimeC() const;
		float GetIntervalTime() const;
		float GetIntervalTimeC() const;

		State GetRunState() const;

	protected:
		void UpdateStates();
		//void SetTime(float time);

	private:
		float m_time;

		float* m_keys;
		int m_keyCount;

		int m_currentInterval;
		State m_currentState;
	};

}
