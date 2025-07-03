#include "Files/FileReader.h"

#include <filesystem>
#include <iostream>
#include <fstream>

namespace Chip
{
	FileReader::FileReader(const std::string& path)
		: m_FilePath(path), m_FileContents("")
	{
		Read();
	}

	FileReader::~FileReader()
	{

	}

	void FileReader::Read()
	{
		std::ifstream file(m_FilePath);
		std::string current;

		if (file.is_open())
		{
			while (std::getline(file, current))
			{
				m_FileContents += current;
				m_FileContents += '\n';
			}
			file.close();
		}
		m_FileContents += '\0';
	}

	const std::string& FileReader::GetContents()
	{
		return m_FileContents;
	}
}