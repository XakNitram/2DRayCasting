workspace "2DRayCasting"
	configurations {
		"Debug",
		"Release",
		"Distribution"
	}

full_configuration = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
project "2DRayCasting"
	location "2DRayCasting"
	kind "WindowedApp"
	language "C++"

	targetdir("bin/" .. full_configuration .. "/%{prj.name}")
	objdir("bin/intermediates/" .. full_configuration .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	include {
		"vendor/GLFW/include",
		"vendor/GLEW/include"
	}

	postbuildcommands {
		("{COPY} ${prj.name}/res/Shaders/ ")
	}

	filter "system:window"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines {

		}

		-- postbuildcommands {
		--     ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. full_configuration .. "/Sandbox")
		-- }


	filter "configurations:Debug"
		defines {
			"_DEBUG"
		}
		symbols "On"

	filter "configurations:Release"
		defines {
			"_DEBUG"
		}
		optimize "On"

	filter "configurations:Distribution"
		defines {
			"NDEBUG"
		}
		optimize "On"
