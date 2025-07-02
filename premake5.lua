workspace "Chip"
	architecture "x86_64"
	startproject "Chip"

	configurations
	{
		"Debug",
		"Release"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["SDL"] = "%{wks.location}/thirdparty/SDL2/include"

group "Dependencies"
	include "thirdparty/SDL2/SDL2"
	include "thirdparty/SDL2/SDL2main"
group ""

include "Chip"