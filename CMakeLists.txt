cmake_minimum_required(VERSION 4.3.4)

project(chip8 C)

add_executable(chip8
  src/main.c
  src/cpu.c
  src/cpu.h
)
