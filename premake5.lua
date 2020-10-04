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
builddir = "bin/%{outputdir}"
intermediatesdir = "bin/intermediates/%{outputdir}"

project "2DRayCasting"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	targetdir(builddir)
	objdir(intermediatesdir)

	files {
		"src/**.h",
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

	defines {

	}

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
