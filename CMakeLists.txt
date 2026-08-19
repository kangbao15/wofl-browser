cmake_minimum_required(VERSION 3.20)

project(WoflBrowser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(WoflBrowser
    src/main.cpp
    engine/engine.cpp
    engine/input.cpp
    engine/blur.cpp
)
