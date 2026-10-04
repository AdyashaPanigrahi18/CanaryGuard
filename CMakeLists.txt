cmake_minimum_required(VERSION 3.16)
project(CanaryMonitor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(canary_monitor
    src/main.cpp
    src/CanaryManager.cpp
    src/FileMonitor.cpp
    src/HashUtils.cpp
    src/Logger.cpp
    src/ConsoleInput.cpp
)

target_include_directories(canary_monitor PRIVATE include)

if (MSVC)
    target_compile_options(canary_monitor PRIVATE /W4)
else()
    target_compile_options(canary_monitor PRIVATE -Wall -Wextra -Wpedantic)
endif()

if (WIN32)
    target_compile_definitions(canary_monitor PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
