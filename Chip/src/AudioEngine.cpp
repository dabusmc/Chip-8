#include "AudioEngine.h"

#include <iostream>

namespace Chip
{
	namespace Waves
	{
		uint16_t Square(uint32_t time, int32_t rate, int32_t freq, double amp)
		{
			auto ticks = rate / freq;
			double cycle = time % ticks;
			return cycle < static_cast<double>(ticks) / 2 ? static_cast<uint16_t>(INT_FAST16_MAX * amp) : 0;
		}
	}

	constexpr uint32_t DEFAULT_FREQUENCY = 500;
	constexpr uint32_t DEFAULT_SAMPLE_RATE = 44100;
	constexpr uint32_t DEFAULT_AMP = 28000;

	AudioEngine::AudioEngine()
		: AudioEngine(DEFAULT_FREQUENCY, DEFAULT_SAMPLE_RATE, DEFAULT_AMP)
	{

	}

	AudioEngine::AudioEngine(int32_t frequency, int32_t rate, int32_t amplitude)
		: m_Frequency(frequency), m_SampleRate(rate), m_Amplitude(amplitude)
	{
		SDL_zero(m_Have);
		SDL_zero(m_Want);

		m_Want.freq = rate;
		m_Want.format = AUDIO_U16;
		m_Want.channels = 1;
		m_Want.samples = 512;
		m_Want.callback = AudioEngine::AudioCallback;
		m_Want.userdata = this;

		m_Device = SDL_OpenAudioDevice(nullptr, 0, &m_Want, &m_Have, 0);
		if (!m_Device)
		{
			std::cout << "Failed to open audio: " << SDL_GetError() << std::endl;
		}
	}

	AudioEngine::~AudioEngine()
	{
		SDL_CloseAudioDevice(m_Device);
	}

	void AudioEngine::Play()
	{
		SDL_PauseAudioDevice(m_Device, 0);
	}

	void AudioEngine::Stop()
	{
		SDL_PauseAudioDevice(m_Device, 1);
	}

	void AudioEngine::AudioCallback(void* data, uint8_t* buffer, int length)
	{
		auto* current = static_cast<AudioEngine*>(data);
		auto* stream = (uint16_t*)buffer;

		for (int i = 0; i < length / 2; i++, current->m_SampleIndex++)
		{
			stream[i] = Waves::Square(current->m_SampleIndex, current->m_SampleRate, current->m_Frequency, current->m_Amplitude);
		}
	}
}