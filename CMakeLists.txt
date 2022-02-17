cmake_minimum_required(VERSION 3.21)
project(chip8 C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(SDL2_PATH $ENV{SDL2_PATH})
set(SDL2_MIXER_PATH $ENV{SDL2_MIXER_PATH})
file(TO_CMAKE_PATH "${SDL2_MIXER_PATH}" SDL2_MIXER_PATH)

set(SDL2_MIXER_INCLUDE_DIR ${SDL2_MIXER_PATH}/include/SDL2)
set(SDL2_MIXER_LIBRARY ${SDL2_MIXER_PATH}/lib/libSDL2_mixer.dll.a)

find_package(SDL2 REQUIRED)

include_directories(
		${PROJECT_SOURCE_DIR}/include
		${SDL2_INCLUDE_DIR}
		${SDL2_MIXER_INCLUDE_DIR}
)
add_executable(
		chip8
		main.c
		src/stack.c
		src/registers.c
		src/cpu.c
		src/instructions.c
		src/screen.c
		src/utils.c
		src/memory.c
		src/debug.c
		src/emulator.c
		src/keyboard.c
		src/timers.c
		src/beeper.c
		src/state.c
)

add_executable(
		chip8_tests
		src/tests/instructions.c
		src/unity.c
)

target_link_libraries(
		chip8
		${SDL2_LIBRARY}
		${SDL2_MIXER_LIBRARY}
)