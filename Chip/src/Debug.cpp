#include "Debug.h"

#include <iostream>
#include <iomanip>

namespace Chip
{
	void Debug::PrintInstruction(uint16_t instruction)
	{
		std::cout << "0x"
			<< std::hex << std::uppercase
			<< std::setw(4) << std::setfill('0')
			<< instruction
			<< std::endl
			<< "\t";

		uint8_t nibOne   = (instruction & 0xF000) >> 12;
		uint8_t nibTwo   = (instruction & 0x0F00) >>  8;
		uint8_t nibThree = (instruction & 0x00F0) >>  4;
		uint8_t nibFour  = (instruction & 0x000F) >>  0;

		switch (nibOne)
		{
		case 0:
		{
			if (instruction == 0x00E0)
			{
				std::cout << "Clear Screen";
			}
			else if (instruction == 0x00EE)
			{
				std::cout << "Return from Subroutine";
			}
			break;
		}
		case 1:
		{
			uint16_t jumpAddr = (nibTwo << 8) | (nibThree << 4) | (nibFour << 0);
			std::cout << "Jump ("
				<< std::hex << std::uppercase
				<< std::setw(4) << std::setfill('0')
				<< jumpAddr << ")";
			break;
		}
		case 2:
		{
			uint16_t jumpAddr = (nibTwo << 8) | (nibThree << 4) | (nibFour << 0);
			std::cout << "Jump to Subroutine("
				<< std::hex << std::uppercase
				<< std::setw(4) << std::setfill('0')
				<< jumpAddr << ")";
			break;
		}
		case 3:
		{
			uint8_t value = (nibThree << 4) | nibFour;
			std::cout << "Skip If V"
				<< std::hex << std::uppercase
				<< (uint16_t)nibTwo << " == " << (uint16_t)value;
			break;
		}
		case 4:
		{
			uint8_t value = (nibThree << 4) | nibFour;
			std::cout << "Skip If V"
				<< std::hex << std::uppercase
				<< (uint16_t)nibTwo << " != " << (uint16_t)value;
			break;
		}
		case 5:
		{
			std::cout << "Skip If V"
				<< std::hex << std::uppercase
				<< (uint16_t)nibTwo << " == V" << (uint16_t)nibThree;
			break;
		}
		case 6:
		{
			uint8_t value = (nibThree << 4) | nibFour;
			std::cout << "Set V"
				<< std::hex << std::uppercase
				<< (uint16_t)nibTwo << " = " << (uint16_t)value;
			break;
		}
		case 7:
		{
			uint8_t value = (nibThree << 4) | nibFour;
			std::cout << "Add "
				<< std::hex << std::uppercase
				<< (uint16_t)value << " to V" << (uint16_t)nibTwo;
			break;
		}
		case 8:
		{
			break;
		}
		case 9:
		{
			std::cout << "Skip If V"
				<< std::hex << std::uppercase
				<< (uint16_t)nibTwo << " != V" << (uint16_t)nibThree;
			break;
		}
		case 0xA:
		{
			uint16_t value = (nibTwo << 8) | (nibThree << 4) | (nibFour << 0);
			std::cout << "Set Index = "
				<< std::hex << std::uppercase
				<< value;
			break;
		}
		case 0xB:
		{
			std::cout << "Jump with Offset";
			break;
		}
		case 0xC:
		{
			std::cout << "Random";
			break;
		}
		case 0xD:
		{
			std::cout << "Display";
			std::cout << "\tN = "
				<< std::hex << std::uppercase
				<< (uint16_t)nibFour << std::endl;
			std::cout << "\tx = V"
				<< std::hex << std::uppercase
				<< (uint16_t)nibTwo << std::endl;
			std::cout << "\ty = V"
				<< std::hex << std::uppercase
				<< (uint16_t)nibThree << std::endl;
			break;
		}
		case 0xE:
		{
			break;
		}
		case 0xF:
		{
			break;
		}
		}

		std::cout << std::endl;
	}
}