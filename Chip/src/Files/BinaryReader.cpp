#include "Files/BinaryReader.h"

#include <cstring>

namespace Chip
{

	BinaryReader::BinaryReader(uint8_t* data, size_t size)
		: m_Data(data), m_Size(size)
	{
		Reset();
	}

	BinaryReader::~BinaryReader()
	{
		delete[] m_Data;
	}

	uint8_t BinaryReader::Next8()
	{
		uint8_t val = m_Data[m_Cursor];
		m_Cursor += 1;
		return val;
	}

	uint16_t BinaryReader::Next16(bool flip)
	{
		uint16_t val = 0;

		uint8_t a;
		uint8_t b;

		if (flip)
		{
			a = m_Data[m_Cursor + 1];
			b = m_Data[m_Cursor + 0];
		}
		else
		{
			a = m_Data[m_Cursor + 0];
			b = m_Data[m_Cursor + 1];
		}

		val = (a << 8 | b << 0);

		m_Cursor += 2;
		return val;
	}

	uint32_t BinaryReader::Next32(bool flip)
	{
		uint32_t val = 0;

		uint8_t a;
		uint8_t b;
		uint8_t c;
		uint8_t d;

		if (flip)
		{
			a = m_Data[m_Cursor + 3];
			b = m_Data[m_Cursor + 2];
			c = m_Data[m_Cursor + 1];
			d = m_Data[m_Cursor + 0];
		}
		else
		{
			a = m_Data[m_Cursor + 0];
			b = m_Data[m_Cursor + 1];
			c = m_Data[m_Cursor + 2];
			d = m_Data[m_Cursor + 3];
		}

		val = (a << 24 | b << 16 | c << 8 | d << 0);

		m_Cursor += 4;
		return val;
	}

	float BinaryReader::NextFloat(bool flip)
	{
		float val = 0.0f;

		uint8_t a;
		uint8_t b;
		uint8_t c;
		uint8_t d;

		if (flip)
		{
			a = m_Data[m_Cursor + 3];
			b = m_Data[m_Cursor + 2];
			c = m_Data[m_Cursor + 1];
			d = m_Data[m_Cursor + 0];
		}
		else
		{
			a = m_Data[m_Cursor + 0];
			b = m_Data[m_Cursor + 1];
			c = m_Data[m_Cursor + 2];
			d = m_Data[m_Cursor + 3];
		}

		uint8_t temp[4] = {a, b, c, d};

		memcpy(&val, temp, sizeof(val));

		m_Cursor += 4;
		return val;
	}

	bool BinaryReader::Check(uint32_t increase)
	{
		return (m_Cursor + increase) <= m_Size;
	}
}