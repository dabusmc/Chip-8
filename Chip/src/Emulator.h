#pragma once

#include <cstdint>
#include <string>
#include <stack>
#include <vector>

#include <SDL.h>

namespace Chip
{
	class Emulator
	{
	public:
		Emulator(const std::string& rom);
		~Emulator();

		void Tick();
		void Step();
		void SwapPixelAt(uint8_t x, uint8_t y);

		uint32_t* GetPixelBuffer() { return m_PixelBuffer; }
		bool IsPaused() { return m_Paused; }

		void Pause();
		void Unpause();

		void DumpRegisters();
		void DumpPC();
		void DumpIndex();
		void DumpTimers();
		void DumpGP();

	private:
		void InitFont();
		void ClearScreen();
		uint8_t AnyKeyPressed();

	private:
		struct Registers
		{
			uint16_t PC;
			uint16_t Index;
			uint8_t DelayTimer;
			uint8_t SoundTimer;
			uint8_t* GeneralPurpose = nullptr;
		};

	private:
		uint32_t* m_PixelBuffer = nullptr;
		uint8_t* m_RAM = nullptr;
		uint16_t m_ProgramSize;
		bool m_Paused;

		std::stack<uint16_t> m_Stack;
		std::vector<SDL_Scancode> m_KeycodeRelationList;
		
		Registers m_Registers;
	};
}