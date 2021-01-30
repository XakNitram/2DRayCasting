workspace "2DRayCasting"
	architecture "x86"
	startproject "2DRayCasting"

	configurations {
		"Debug", "Release"
	}

	flags {
		"MultiProcessorCompile"
	}

-- %{cfg.system} and %{cfg.architecture} if the need arises.
outputdir = "%{cfg.buildcfg}/%{prj.name}"
outputbase = "%{_WORKING_DIR}/bin"
builddir = "%{outputbase}/%{outputdir}"
intermediatesdir = "%{outputbase}/intermediates/%{outputdir}"

project "2DRayCasting"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	targetdir(builddir)
	objdir(intermediatesdir)
	debugdir "%{builddir}"

	postbuildcommands {
		"{MKDIR} %{cfg.targetdir}/Shaders/",
		"{COPY} res/Shaders/ %{cfg.targetdir}/Shaders/"
	}

	pchheader "pch.hpp"
	pchsource "src/pch.cpp"

	files {
		"src/**.hpp",
		"src/**.cpp",
		"res/Shaders/**.vert",
		"res/Shaders/**.frag"
	}

	includedirs {
		"src",
		"vendor/GLFW/include",
		"vendor/Glad/include"
	}

	links {
		"GLFW",
		"Glad"
	}

	filter "system:windows"
		systemversion "latest"
		links { "OpenGL32" }

	filter "system:not windows"
		links { "GL" }

	filter "configurations:Debug"
		defines { "_DEBUG" }
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		runtime "Release"
		optimize "Full"

include "vendor/GLFW"
include "vendor/Glad"
