#include "Emulator.h"

#include "Debug.h"

#include "Files/FileUtils.h"
#include "Files/BinaryFileReader.h"

#include <iostream>
#include <iomanip>
#include <cmath>

#define KB 1024

namespace Chip
{
	const int SCREEN_WIDTH = 64;
	const int SCREEN_HEIGHT = 32;

	const uint32_t WHITE = 0xFFFFFFFF;
	const uint32_t BLACK = 0xFF000000;

	const uint16_t PROGRAM_ADDRESS = 0x200;
	const uint16_t FONT_OFFSET = 0x50;

	inline uint32_t RGBAToInt(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		return (r << 0) | (g << 8) | (b << 16) | (a << 24);
	}

	Emulator::Emulator(const std::string& rom)
	{
		std::string romPath = FileUtils::GetCWD() + "/roms/" + rom + ".ch8";
		BinaryFileReader reader(romPath);
		m_RAM = new uint8_t[4 * KB];

		m_Paused = false;

		m_ProgramSize = reader.GetSize();
		memcpy(m_RAM + PROGRAM_ADDRESS, reader.GetContents(), m_ProgramSize);

		m_Registers.PC = PROGRAM_ADDRESS;
		m_Registers.DelayTimer = 0;
		m_Registers.SoundTimer = 0;
		m_Registers.Index = 0;
		m_Registers.GeneralPurpose = new uint8_t[16];


		/*
			1	2	3	C
			4	5	6	D
			7	8	9	E
			A	0	B	F

			1	2	3	4
			Q	W	E	R
			A	S	D	F
			Z	X	C	V
		*/

		m_KeycodeRelationList = std::vector<SDL_Scancode>
		{
			// 0
			SDL_SCANCODE_X,
			// 1
			SDL_SCANCODE_1,
			// 2
			SDL_SCANCODE_2,
			// 3
			SDL_SCANCODE_3,
			// 4
			SDL_SCANCODE_Q,
			// 5
			SDL_SCANCODE_W,
			// 6
			SDL_SCANCODE_E,
			// 7
			SDL_SCANCODE_A,
			// 8
			SDL_SCANCODE_S,
			// 9
			SDL_SCANCODE_D,
			// A
			SDL_SCANCODE_Z,
			// B
			SDL_SCANCODE_C,
			// C
			SDL_SCANCODE_4,
			// D
			SDL_SCANCODE_R,
			// E
			SDL_SCANCODE_F,
			// F
			SDL_SCANCODE_V
		};

		InitFont();

		m_PixelBuffer = new uint32_t[SCREEN_WIDTH * SCREEN_HEIGHT];
		ClearScreen();
	}

	Emulator::~Emulator()
	{
		delete[] m_Registers.GeneralPurpose;
		delete[] m_PixelBuffer;
		delete[] m_RAM;
	}

	void Emulator::Tick()
	{
		if (m_Paused)
			return;
		Step();
	}

	void Emulator::Step()
	{
		if (m_Registers.DelayTimer > 0)
		{
			m_Registers.DelayTimer -= 1;
		}

		if (m_Registers.SoundTimer > 0)
		{
			m_Registers.SoundTimer -= 1;
		}

		// Fetch
		uint8_t instructionA = m_RAM[m_Registers.PC];
		uint8_t instructionB = m_RAM[m_Registers.PC + 1];
		m_Registers.PC += 2;
		uint16_t instruction = (instructionA << 8) | instructionB;

		if (m_Paused)
		{
			Debug::PrintInstruction(instruction);
		}

		// Decode
		uint8_t nibOne = (instructionA & 0xF0) >> 4;
		uint8_t nibTwo = (instructionA & 0x0F);
		uint8_t nibThree = (instructionB & 0xF0) >> 4;
		uint8_t nibFour = (instructionB & 0x0F);

		// Execute
		switch (nibOne)
		{
		case 0:
		{
			if (instruction == 0x00E0)
			{
				ClearScreen();
			}
			else if (instruction == 0x00EE)
			{
				uint16_t returnAddress = m_Stack.top();
				m_Stack.pop();
				m_Registers.PC = returnAddress;
			}
			break;
		}
		case 1:
		{
			uint16_t jumpAddr = (nibTwo << 8) | (nibThree << 4) | (nibFour << 0);
			m_Registers.PC = jumpAddr;
			break;
		}
		case 2:
		{
			uint16_t jumpAddr = (nibTwo << 8) | (nibThree << 4) | (nibFour << 0);
			m_Stack.push(m_Registers.PC);
			m_Registers.PC = jumpAddr;
			break;
		}
		case 3:
		{
			uint8_t value = (nibThree << 4) | nibFour;
			if (m_Registers.GeneralPurpose[nibTwo] == value)
			{
				m_Registers.PC += 2;
			}
			break;
		}
		case 4:
		{
			uint8_t value = (nibThree << 4) | nibFour;
			if (m_Registers.GeneralPurpose[nibTwo] != value)
			{
				m_Registers.PC += 2;
			}
			break;
		}
		case 5:
		{
			if (m_Registers.GeneralPurpose[nibTwo] == m_Registers.GeneralPurpose[nibThree])
			{
				m_Registers.PC += 2;
			}
			break;
		}
		case 6:
		{
			uint8_t value = (nibThree << 4) | nibFour;
			m_Registers.GeneralPurpose[nibTwo] = value;
			break;
		}
		case 7:
		{
			uint8_t value = (nibThree << 4) | nibFour;
			m_Registers.GeneralPurpose[nibTwo] += value;
			break;
		}
		case 8:
		{
			switch (nibFour)
			{
			case 0:
				m_Registers.GeneralPurpose[nibTwo] = m_Registers.GeneralPurpose[nibThree];
				break;
			case 1:
				m_Registers.GeneralPurpose[nibTwo] |= m_Registers.GeneralPurpose[nibThree];
				break;
			case 2:
				m_Registers.GeneralPurpose[nibTwo] &= m_Registers.GeneralPurpose[nibThree];
				break;
			case 3:
				m_Registers.GeneralPurpose[nibTwo] ^= m_Registers.GeneralPurpose[nibThree];
				break;
			case 4:
				if ((uint16_t)m_Registers.GeneralPurpose[nibTwo] + (uint16_t)m_Registers.GeneralPurpose[nibThree] > 255)
				{
					m_Registers.GeneralPurpose[15] = 1;
				}
				else
				{
					m_Registers.GeneralPurpose[15] = 0;
				}

				m_Registers.GeneralPurpose[nibTwo] += m_Registers.GeneralPurpose[nibThree];

				break;
			case 5:
				if (m_Registers.GeneralPurpose[nibTwo] > m_Registers.GeneralPurpose[nibThree])
				{
					m_Registers.GeneralPurpose[15] = 1;
				}
				else
				{
					m_Registers.GeneralPurpose[15] = 0;
				}

				m_Registers.GeneralPurpose[nibTwo] -= m_Registers.GeneralPurpose[nibThree];

				break;
			case 6:
				m_Registers.GeneralPurpose[nibTwo] = m_Registers.GeneralPurpose[nibThree];
				m_Registers.GeneralPurpose[15] = (m_Registers.GeneralPurpose[nibTwo] >> 0) & 1;
				m_Registers.GeneralPurpose[nibTwo] >>= 1;
				break;
			case 7:
				if (m_Registers.GeneralPurpose[nibThree] > m_Registers.GeneralPurpose[nibTwo])
				{
					m_Registers.GeneralPurpose[15] = 1;
				}
				else
				{
					m_Registers.GeneralPurpose[15] = 0;
				}

				m_Registers.GeneralPurpose[nibTwo] = m_Registers.GeneralPurpose[nibThree] - m_Registers.GeneralPurpose[nibTwo];

				break;
			case 0xE:
				m_Registers.GeneralPurpose[nibTwo] = m_Registers.GeneralPurpose[nibThree];
				m_Registers.GeneralPurpose[15] = (m_Registers.GeneralPurpose[nibTwo] >> 8) & 1;
				m_Registers.GeneralPurpose[nibTwo] <<= 1;
				break;
			}
			break;
		}
		case 9:
		{
			if (m_Registers.GeneralPurpose[nibTwo] != m_Registers.GeneralPurpose[nibThree])
			{
				m_Registers.PC += 2;
			}
			break;
		}
		case 0xA:
		{
			uint16_t value = (nibTwo << 8) | (nibThree << 4) | (nibFour << 0);
			m_Registers.Index = value;
			break;
		}
		case 0xB:
		{
			uint16_t value = (nibTwo << 8) | (nibThree << 4) | (nibFour << 0);
			value += m_Registers.GeneralPurpose[0];
			m_Registers.PC = value;
			break;
		}
		case 0xC:
		{
			// RANDOM NUMBER
			break;
		}
		case 0xD:
		{
			// DISPLAY INSTRUCTION
			uint8_t xInit = m_Registers.GeneralPurpose[nibTwo] & (SCREEN_WIDTH - 1);
			uint8_t y = m_Registers.GeneralPurpose[nibThree] & (SCREEN_HEIGHT - 1);
			m_Registers.GeneralPurpose[15] = 0;

			for (auto i = 0; i < nibFour; i++)
			{
				if (y >= SCREEN_HEIGHT)
				{
					break;
				}

				uint8_t currentByte = m_RAM[m_Registers.Index + i];
				uint8_t x = xInit;

				for (auto mask = 0x80; mask != 0; mask >>= 1)
				{
					if (x >= SCREEN_WIDTH)
					{
						break;
					}

					if (currentByte & mask)
					{
						// The current bit is 1
						if (m_PixelBuffer[y * SCREEN_WIDTH + x] == WHITE)
						{
							m_Registers.GeneralPurpose[15] = 1;
						}
						SwapPixelAt(x, y);
					}

					x += 1;
				}

				y += 1;
			}

			break;
		}
		case 0xE:
		{
			if (instructionB == 0x9E)
			{
				const uint8_t* keystates = SDL_GetKeyboardState(nullptr);
				if (keystates[m_KeycodeRelationList[m_Registers.GeneralPurpose[nibTwo]]])
				{
					m_Registers.PC += 2;
				}
			}
			else if (instructionB == 0xA1)
			{
				const uint8_t* keystates = SDL_GetKeyboardState(nullptr);
				if (!keystates[m_KeycodeRelationList[m_Registers.GeneralPurpose[nibTwo]]])
				{
					m_Registers.PC += 2;
				}
			}

			break;
		}
		case 0xF:
		{
			switch (instructionB)
			{
			case 0x07:
				m_Registers.GeneralPurpose[nibTwo] = m_Registers.DelayTimer;
				break;
			case 0x15:
				m_Registers.DelayTimer = m_Registers.GeneralPurpose[nibTwo];
				break;
			case 0x18:
				m_Registers.SoundTimer = m_Registers.GeneralPurpose[nibTwo];
				break;
			case 0x1E:
				m_Registers.GeneralPurpose[15] = (m_Registers.Index + m_Registers.GeneralPurpose[nibTwo]) >= 0x1000 ? 1 : 0;
				m_Registers.Index += m_Registers.GeneralPurpose[nibTwo];
				break;
			case 0x0A:
			{
				uint8_t key = AnyKeyPressed();
				if (key == 20)
				{
					m_Registers.PC -= 2;
				}
				else
				{
					m_Registers.GeneralPurpose[nibTwo] = key;
				}
				break;
			}
			case 0x29:
			{
				uint8_t character = m_Registers.GeneralPurpose[nibTwo] & 0x0F;
				m_Registers.Index = FONT_OFFSET + (5 * character);
				break;
			}
			case 0x33:
			{
				uint8_t value = m_Registers.GeneralPurpose[nibTwo];
				if (value == 0)
				{
					m_RAM[m_Registers.Index] = 0;
				}
				else
				{
					int size = trunc(log10(value)) + 1;
					for (int i = 0; i < size; i++)
					{
						m_RAM[m_Registers.Index + i] = value;
						value /= 10;
					}
				}
				break;
			}
			case 0x55:
			{
				if (nibTwo == 0)
				{
					m_RAM[m_Registers.Index] = m_Registers.GeneralPurpose[0];
				}
				else
				{
					for (auto i = 0; i < nibTwo; i++)
					{
						m_RAM[m_Registers.Index + i] = m_Registers.GeneralPurpose[i];
					}
				}
				break;
			}
			case 0x65:
			{
				if (nibTwo == 0)
				{
					m_Registers.GeneralPurpose[0] = m_RAM[m_Registers.Index];
				}
				else
				{
					for (auto i = 0; i < nibTwo; i++)
					{
						m_Registers.GeneralPurpose[i] = m_RAM[m_Registers.Index + i];
					}
				}
				break;
			}
			default:
				break;
			}
			break;
		}
		}
	}

	void Emulator::Pause()
	{
		m_Paused = true;
	}

	void Emulator::Unpause()
	{
		m_Paused = false;
	}

	void Emulator::DumpRegisters()
	{
		std::cout << "Registers" << std::endl;
		DumpPC();
		DumpIndex();
		DumpTimers();
		DumpGP();
	}

	void Emulator::DumpPC()
	{
		std::cout << "Program Counter: "
			<< std::hex << std::uppercase
			<< std::setw(4) << std::setfill('0')
			<< m_Registers.PC << std::endl;
	}

	void Emulator::DumpIndex()
	{
		std::cout << "Index: 0x"
			<< std::hex << std::uppercase
			<< std::setw(4) << std::setfill('0')
			<< m_Registers.Index << std::endl;
	}

	void Emulator::DumpTimers()
	{
		std::cout << "Delay Timer: " << std::to_string(m_Registers.DelayTimer) << std::endl;
		std::cout << "Sound Timer: " << std::to_string(m_Registers.SoundTimer) << std::endl;
	}

	void Emulator::DumpGP()
	{
		for (auto i = 0; i < 16; i++)
		{
			std::cout << "V" << i << ": "
				<< std::hex << std::uppercase
				<< std::setw(2) << std::setfill('0')
				<< (uint16_t)m_Registers.GeneralPurpose[i] << std::endl;
		}
	}

	void Emulator::SwapPixelAt(uint8_t x, uint8_t y)
	{
		if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
		{
			uint32_t current = m_PixelBuffer[y * SCREEN_WIDTH + x];
			if (current == WHITE)
			{
				m_PixelBuffer[y * SCREEN_WIDTH + x] = BLACK;
			}
			else
			{
				m_PixelBuffer[y * SCREEN_WIDTH + x] = WHITE;
			}
		}
	}

	void Emulator::InitFont()
	{
		uint8_t* font = new uint8_t[80]
		{
			0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
			0x20, 0x60, 0x20, 0x20, 0x70, // 1
			0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
			0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
			0x90, 0x90, 0xF0, 0x10, 0x10, // 4
			0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
			0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
			0xF0, 0x10, 0x20, 0x40, 0x40, // 7
			0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
			0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
			0xF0, 0x90, 0xF0, 0x90, 0x90, // A
			0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
			0xF0, 0x80, 0x80, 0x80, 0xF0, // C
			0xE0, 0x90, 0x90, 0x90, 0xE0, // D
			0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
			0xF0, 0x80, 0xF0, 0x80, 0x80  // F
		};

		memcpy(m_RAM + FONT_OFFSET, font, 80 * sizeof(uint8_t));
		delete[] font;
	}

	void Emulator::ClearScreen()
	{
		for (auto i = 0; i < SCREEN_WIDTH; i++)
		{
			for (auto j = 0; j < SCREEN_HEIGHT; j++)
			{
				m_PixelBuffer[j * SCREEN_WIDTH + i] = BLACK;
			}
		}
	}

	uint8_t Emulator::AnyKeyPressed()
	{
		const uint8_t* keystates = SDL_GetKeyboardState(nullptr);
		for (uint8_t i = 0; i < m_KeycodeRelationList.size(); i++)
		{
			if (keystates[m_KeycodeRelationList[i]])
			{
				return i;
			}
		}

		return 20;
	}
}