cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_STANDARD 11)
set(CIMGUI_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../cimgui/)
set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}scripts/buildsystems/vcpkg.cmake)

project(cimgui)
#my copy pasta is esquiste
if(WIN32) #to mingw work as all the others
set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif(WIN32)
if (EXISTS ${CIMGUI_ROOT}imgui/backends)
	set(BAKENDS_FOLDER "${CIMGUI_ROOT}imgui/backends/")
else()
	set(BAKENDS_FOLDER "${CIMGUI_ROOT}imgui/examples/")
	endif()
	
if (EXISTS ${CIMGUI_ROOT}imgui/imgui_tables.cpp)
set(TABLES_SOURCE "${CIMGUI_ROOT}imgui/imgui_tables.cpp")
else()
set(TABLES_SOURCE "")
endif()

if (WIN32)
    add_definitions("-DIMGUI_IMPL_API=extern \"C\" __declspec\(dllexport\)")
	else(WIN32)
    add_definitions("-DIMGUI_IMPL_API=extern \"C\" ")
endif(WIN32)

find_package(SDL2 CONFIG REQUIRED)
add_library(cimgui_sdl SHARED 
${CIMGUI_ROOT}cimgui.cpp 
${CIMGUI_ROOT}imgui/imgui.cpp 
${CIMGUI_ROOT}imgui/imgui_draw.cpp 
${CIMGUI_ROOT}imgui/imgui_demo.cpp 
${CIMGUI_ROOT}imgui/imgui_widgets.cpp
${TABLES_SOURCE} 
${BAKENDS_FOLDER}imgui_impl_opengl3.cpp
${CIMGUI_ROOT}imgui/examples/libs/gl3w/GL/gl3w.c
${CIMGUI_ROOT}backend_test/cimgui_extras.cpp
${BAKENDS_FOLDER}imgui_impl_sdl.cpp
)
add_definitions("-DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1")
add_compile_definitions("IMGUI_IMPL_OPENGL_LOADER_GL3W")
target_link_libraries(cimgui_sdl opengl32  SDL2::SDL2)
include_directories(${CIMGUI_ROOT} ${CIMGUI_ROOT}imgui  ${CIMGUI_ROOT}generator/output ${CIMGUI_ROOT}imgui/examples/libs/gl3w ${CIMGUI_ROO}backend_test)


project(Client)

set(GAMETOOLS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../GameTools)
file(GLOB GAMETOOLS_SRC ${GAMETOOLS_ROOT}/*.c)
file(GLOB CLIENT_SRC *.c)
list(REMOVE_ITEM CLIENT_SRC cimgui_extras.cpp)
add_executable(${PROJECT_NAME} ${CLIENT_SRC} ${GAMETOOLS_SRC})

find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-net CONFIG REQUIRED)
find_package(Lua 5.4.1 REQUIRED)


set(CIMGUI_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../cimgui/)
set(CIMGUI_SRC ${CIMGUI_ROOT} ${CIMGUI_ROOT}generator/output ${CIMGUI_ROOT}backend_test)
if(MSVC)
	find_path(DIRENT_INCLUDE_DIRS "dirent.h")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_net SDL2::SDL2_net ${LUA_LIBRARIES} cimgui_sdl)
target_include_directories(${PROJECT_NAME} PRIVATE ${LUA_INCLUDE_DIR} ${DIRENT_INCLUDE_DIRS} ${GAMETOOLS_ROOT} ${CIMGUI_SRC})