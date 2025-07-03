#include "Files/BinaryFileReader.h"

#include <filesystem>
#include <iostream>
#include <fstream>

namespace Chip
{
	BinaryFileReader::BinaryFileReader()
		: m_FilePath(""), m_Reader(nullptr)
	{
	}

	BinaryFileReader::BinaryFileReader(const std::string& path)
		: m_FilePath(path), m_Reader(nullptr)
	{
		Read();
	}

	BinaryFileReader::~BinaryFileReader()
	{
		delete m_Reader;
	}

	void BinaryFileReader::SetPath(const std::string& path)
	{
		if (m_Reader == nullptr)
		{
			m_FilePath = path;
			Read();
		}
	}

	void BinaryFileReader::Read()
	{
		std::ifstream file(m_FilePath, std::ios::ate | std::ios::binary);

		size_t fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		uint8_t* data = new uint8_t[fileSize];
		file.read(reinterpret_cast<char*>(data), fileSize);
		file.close();

		m_Reader = new BinaryReader(data, fileSize);
	}

	uint8_t BinaryFileReader::Next8()
	{
		return m_Reader->Next8();
	}

	uint16_t BinaryFileReader::Next16(bool flip)
	{
		return m_Reader->Next16(flip);
	}

	uint32_t BinaryFileReader::Next32(bool flip)
	{
		return m_Reader->Next32(flip);
	}

	float BinaryFileReader::NextFloat(bool flip)
	{
		return m_Reader->NextFloat(flip);
	}
}