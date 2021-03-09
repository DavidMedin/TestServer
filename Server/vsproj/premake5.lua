workspace"Server"
	configurations {"Debug","Release"}
	architecture "x86_64"
project "Server"
	location "."
	objdir "./tmp"
	kind "ConsoleApp"
	language "C"
	targetdir "../windows"
	debugdir "../"
	
	files {"../*.c","../*.h","../../*.c","../../*.h"}--includes all the c/h files
--,"Volcano/libs/SPIRV-Reflect/spirv_reflect.c","Volcano/libs/SPIRV-Reflect/spirv_reflect.h"
	-- group "Spirv-Reflect"
	-- 	files {"Volcano/src/libs/SPIRV-Reflect/*.*"}
	-- group ""
	-- includedirs{"."}
	libdirs {"$(VcpkgCurrentInstalledDir)$(VscpkgConfigSubdir)lib/manual-link/"}
	links{"SDL2main","SDL2d","SDL2_net","lua"}

	filter "Debug"
		defines {"_DEBUG","_CONSOLE"}
		symbols "On"
	filter "Release"
		defines{"NDEBUG"}
		optimize "On"