#pragma once

#include <cstdint>
#include <string>

#include "Files/BinaryReader.h"

// NOTE: Imported from another project
namespace Chip
{
	class BinaryFileReader
	{
	public:
		BinaryFileReader();
		BinaryFileReader(const std::string& path);
		~BinaryFileReader();

		void SetPath(const std::string& path);

		uint8_t* GetContents() const { return m_Reader->GetContents(); }
		size_t GetSize() const { return m_Reader->GetSize(); }

		uint8_t Next8();
		uint16_t Next16(bool flip);
		uint32_t Next32(bool flip);
		float NextFloat(bool flip);
		
		void Move(size_t cursorPos) { m_Reader->Move(cursorPos); }
		void Reset() { m_Reader->Reset(); }
		size_t GetCursor() const { return m_Reader->GetCursor(); }

	private:
		void Read();

	private:
		std::string m_FilePath;
		BinaryReader* m_Reader = nullptr;
	};
}