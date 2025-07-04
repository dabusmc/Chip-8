project "Chip"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"src",
		"%{IncludeDir.SDL}",
	}

	links
	{
		"SDL2",
		"SDL2main"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "CHIP_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "CHIP_RELEASE"
		runtime "Release"
		optimize "on"