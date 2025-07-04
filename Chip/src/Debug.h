#pragma once

#include <cstdint>

namespace Chip
{
	class Debug
	{
	public:
		static void PrintInstruction(uint16_t instruction);
	};
}