Project(cimgui_sdl)
cmake_minimum_required(VERSION 3.1)
if(WIN32) #to mingw work as all the others
set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif(WIN32)
#general settings
set(CIMGUI_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../cimgui/)

if (EXISTS ${CIMGUI_ROOT}/imgui/backends)
	set(BAKENDS_FOLDER ${CIMGUI_ROOT}imgui/backends/)
else()
	set(BAKENDS_FOLDER ${CIMGUI_ROOT}imgui/examples/)
endif()

if (EXISTS ${CIMGUI_ROOT}imgui/imgui_tables.cpp)
	set(TABLES_SOURCE ${CIMGUI_ROOT}imgui/imgui_tables.cpp)
else()
	set(TABLES_SOURCE "")
endif()

include_directories(${CIMGUI_ROOT}imgui)
add_definitions("-DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1")

include_directories(${CIMGUI_ROOT})
set(IMGUI_SOURCES ${CIMGUI_ROOT}cimgui.cpp 
${CIMGUI_ROOT}imgui/imgui.cpp 
${CIMGUI_ROOT}imgui/imgui_draw.cpp 
${CIMGUI_ROOT}imgui/imgui_demo.cpp 
${CIMGUI_ROOT}imgui/imgui_widgets.cpp
${TABLES_SOURCE} 
)

set(IMGUI_SOURCES_sdl)
set(IMGUI_LIBRARIES )

if (WIN32)
    add_definitions("-DIMGUI_IMPL_API=extern \"C\" __declspec\(dllexport\)")
else(WIN32)
    add_definitions("-DIMGUI_IMPL_API=extern \"C\" ")
endif(WIN32)

add_compile_definitions("IMGUI_IMPL_OPENGL_LOADER_GL3W")

#optional adding freetype
option(IMGUI_FREETYPE "add Freetype2" OFF)

if(IMGUI_FREETYPE)
	FIND_PACKAGE(freetype REQUIRED PATHS ${FREETYPE_PATH})
	list(APPEND IMGUI_LIBRARIES freetype)
	list(APPEND IMGUI_SOURCES ${CIMGUI_ROOT}imgui/misc/freetype/imgui_freetype.cpp)
	add_definitions("-DCIMGUI_FREETYPE=1")
endif(IMGUI_FREETYPE)

#opengl3
list(APPEND IMGUI_SOURCES ${BAKENDS_FOLDER}imgui_impl_opengl3.cpp ${CIMGUI_ROOT}imgui/examples/libs/gl3w/GL/gl3w.c)
list(APPEND IMGUI_SOURCES ${CIMGUI_ROOT}/backend_test/cimgui_extras.cpp)
include_directories(${CIMGUI_ROOT}imgui/examples/libs/gl3w)
if(WIN32)
       list(APPEND IMGUI_LIBRARIES opengl32)
else(WIN32)#Unix
       list(APPEND IMGUI_LIBRARIES GL)
endif(WIN32)

set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}scripts/buildsystems/vcpkg.cmake)
find_package(SDL2 CONFIG REQUIRED)
#sdl2
list(APPEND IMGUI_SOURCES ${BAKENDS_FOLDER}imgui_impl_sdl.cpp)
# if(DEFINED SDL_PATH)
# 	message(STATUS "SDL_PATH defined as " ${SDL_PATH})
# 	FIND_PACKAGE(SDL2 REQUIRED PATHS ${SDL_PATH})
# 	get_target_property(SDL_INCLUDE SDL2::SDL2 INTERFACE_INCLUDE_DIRECTORIES)
# 	message(STATUS "sdlinclude is " ${SDL_INCLUDE})
# 	if ("${SDL_INCLUDE}" STREQUAL "" OR "${SDL_INCLUDE}" STREQUAL "SDL_INCLUDE-NOTFOUND") #if not found latest SDL2 cmake config use older
# 		message(STATUS "sdlinclude2 is " ${SDL2_INCLUDE_DIRS})
# 		include_directories(${SDL2_INCLUDE_DIRS})
# 		set(IMGUI_SDL_LIBRARY ${SDL2_LIBRARIES})
# 		message(STATUS IMGUI_SDL_LIBRARY ${SDL2_LIBRARIES})
# 	else()#use new one SDL2 config
		# include_directories(${SDL_INCLUDE})
		set(IMGUI_SDL_LIBRARY SDL2::SDL2)
		set(SDL_MAIN SDL2::SDL2main)
		message(STATUS ${SDL_MAIN} ${IMGUI_SDL_LIBRARY})
# 	endif()
# else(DEFINED SDL_PATH)
# 	message(STATUS "SDL_PATH not defined")
# 	set(IMGUI_SDL_LIBRARY SDL2)
# endif(DEFINED SDL_PATH)
# find_package(gl3w CONFIG REQUIRED)

add_library(cimgui_sdl SHARED ${IMGUI_SOURCES})
target_link_libraries(cimgui_sdl ${IMGUI_LIBRARIES} ${IMGUI_SDL_LIBRARY} )

#using library
# include_directories(${CIMGUI_ROOT}generator/output/)
# add_executable(test_sdl main.c)
# if (MINGW)
# target_link_options(test_sdl PRIVATE "-mconsole")
# endif()
# target_link_libraries(test_sdl ${IMGUI_SDL_LIBRARY} cimgui_sdl)

