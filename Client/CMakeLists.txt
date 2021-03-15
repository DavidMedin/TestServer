cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_STANDARD 11)
set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}scripts/buildsystems/vcpkg.cmake)
project(Client)

set(GAMETOOLS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../GameTools)
file(GLOB GAMETOOLS_SRC ${GAMETOOLS_ROOT}/*.c)
file(GLOB CLIENT_SRC *.c)
add_executable(${PROJECT_NAME} ${CLIENT_SRC} ${GAMETOOLS_SRC})

find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-net CONFIG REQUIRED)
find_package(Lua 5.4.1 REQUIRED)


if(MSVC)
	find_path(DIRENT_INCLUDE_DIRS "dirent.h")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_net SDL2::SDL2_net ${LUA_LIBRARIES})
# target_include_directories(${PROJECT_NAME} PRIVATE ${GAMETOOLS_ROOT} ${LUA_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${LUA_INCLUDE_DIR} ${DIRENT_INCLUDE_DIRS} ${GAMETOOLS_ROOT})