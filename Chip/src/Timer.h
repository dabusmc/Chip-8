#pragma once

#include <cstdint>

namespace Chip
{
	class Timer
	{
	public:
		Timer();

		void Start();
		void Stop();
		void Pause();
		void Unpause();

		uint32_t GetTicks();

		bool HasStarted() { return m_Started; }
		bool isPaused() { return m_Paused; }

	private:
		uint32_t m_StartTicks;
		uint32_t m_PausedTicks;

		bool m_Started;
		bool m_Paused;
	};
}