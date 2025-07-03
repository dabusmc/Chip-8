#pragma once

#include <cstdint>

// NOTE: Imported from another project
namespace Chip
{
	class BinaryReader
	{
	public:
		BinaryReader(uint8_t* data, size_t size);
		~BinaryReader();

		uint8_t* GetContents() const { return m_Data; }
		size_t GetSize() const { return m_Size; }

		uint8_t Next8();
		uint16_t Next16(bool flip);
		uint32_t Next32(bool flip);
		float NextFloat(bool flip);

		void Move(size_t cursorPos) { m_Cursor += cursorPos; }
		void Reset() { m_Cursor = 0; }
		size_t GetCursor() const { return m_Cursor; }

	private:
		bool Check(uint32_t increase);

	private:
		size_t m_Cursor;
		uint8_t* m_Data;
		size_t m_Size;
	};
}