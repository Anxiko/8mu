cmake_minimum_required(VERSION 3.21)
project(chip8 C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(SDL2_PATH $ENV{SDL2_PATH})
find_package(SDL2 REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include ${SDL2_INCLUDE_DIR})
add_executable(chip8
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
        )

message(${SDL2_LIBRARY})
target_link_libraries(chip8 ${SDL2_LIBRARY})