#pragma once

#include <SDL.h>

namespace Chip
{
	class AudioEngine
	{
	public:
		AudioEngine();
		AudioEngine(int32_t frequency, int32_t rate, int32_t amplitude);
		~AudioEngine();

		void Play();
		void Stop();

	private:
		static void AudioCallback(void* data, uint8_t* buffer, int length);

	private:
		int32_t m_SampleRate, m_Frequency, m_Amplitude;
		int32_t m_SampleIndex;

		SDL_AudioSpec m_Have, m_Want;
		SDL_AudioDeviceID m_Device;
	};
}