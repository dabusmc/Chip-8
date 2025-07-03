#pragma once

#include <cstdint>
#include <string>
#include <stack>

namespace Chip
{
	class Emulator
	{
	public:
		Emulator(const std::string& rom);
		~Emulator();

		void Tick(float deltaTime);
		void SwapPixelAt(uint8_t x, uint8_t y);

		uint32_t* GetPixelBuffer() { return m_PixelBuffer; }

	private:
		void InitFont();
		void ClearScreen();

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
		bool temp;

		std::stack<uint16_t> m_Stack;
		
		Registers m_Registers;
	};
}