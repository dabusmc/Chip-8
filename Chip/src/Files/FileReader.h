#pragma once

#include <string>

// NOTE: Imported from another project
namespace Chip
{
	class FileReader
	{	
	public:
		FileReader(const std::string& path);
		~FileReader();

		const std::string& GetContents();

	private:
		void Read();

	private:
		std::string m_FilePath;
		std::string m_FileContents;
	};
}