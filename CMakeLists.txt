cmake_minimum_required(VERSION 3.10)
project(Task LANGUAGES C)

add_executable(Task main.c)

option(DEBUG_MAP "Добавление debug map" OFF)
if(DEBUG_MAP)
    target_compile_definitions(Task PRIVATE DEBUG_MAP=1)
endif()

# Настройка UTF-8
if(MSVC)
    target_compile_options(Task PRIVATE /utf-8)
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(Task PRIVATE
        -finput-charset=UTF-8
        -fexec-charset=UTF-8
    )
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_compile_options(Task PRIVATE -finput-charset=UTF-8)
endif()