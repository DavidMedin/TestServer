cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_STANDARD 11)
set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}scripts/buildsystems/vcpkg.cmake)
project(Server)
add_executable(Server server.c)

find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-net CONFIG REQUIRED)
find_package(Lua 5.4.1 REQUIRED)


if(MSVC)
	find_path(DIRENT_INCLUDE_DIRS "dirent.h")
endif()

target_link_libraries(Server PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_net ${LUA_LIBRARIES})
target_include_directories(Server PRIVATE ${LUA_INCLUDE_DIR} ${DIRENT_INCLUDE_DIRS})