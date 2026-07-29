cmake_minimum_required(VERSION 4.3.4)

project(chip8 C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

add_executable(chip8
  src/main.c
  src/cpu.c
)

target_link_libraries(chip8 PRIVATE SDL3::SDL3)
