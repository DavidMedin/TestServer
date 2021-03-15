cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_STANDARD 11)
set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}scripts/buildsystems/vcpkg.cmake)
project(Server)

#adds GameTools
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../GameTools ${CMAKE_CURRENT_BINARY_DIR}/GameTools-build)

set(GAMETOOLS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../GameTools)
file(GLOB GAMETOOLS_SRC ${GAMETOOLS_ROOT}/*.c)
file(GLOB SERVER_SRC *.c)
add_executable(Server ${SERVER_SRC} ${GAMETOOLS_SRC})

find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-net CONFIG REQUIRED)
find_package(Lua 5.4.1 REQUIRED)
message(STATUS ${GameTools_Root})

if(MSVC)
	find_path(DIRENT_INCLUDE_DIRS "dirent.h")
	set_target_properties(Server PROPERTIES 
    VS_DEBUGGER_WORKING_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR})
endif()

target_link_libraries(Server PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_net ${LUA_LIBRARIES})
target_include_directories(Server PRIVATE ${LUA_INCLUDE_DIR} ${DIRENT_INCLUDE_DIRS} ${GAMETOOLS_ROOT})