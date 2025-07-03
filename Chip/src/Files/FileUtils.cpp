#include "Files/FileUtils.h"

#include <filesystem>

namespace Chip
{
	std::string FileUtils::GetCWD()
	{
		std::filesystem::path cwd = std::filesystem::current_path();
		return cwd.string();
	}
}