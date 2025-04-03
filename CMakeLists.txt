cmake_minimum_required(VERSION 3.21.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)

project(WaveshareDrives)

file(GLOB SOURCES 
    "lib/Config/Dev_Config.c"
    "lib/Driver/*.c"
    "lib/EPD/*.c"
    "lib/Fonts/*.c"
    "lib/GUI/*.c"
)

file(GLOB HEADERS
    "lib/Config/Debug.h"
    "lib/Config/Dev_Config.h"
    "lib/Driver/*.h"
    "lib/EPD/*.h"
    "lib/Fonts/*.h"
    "lib/GUI/*.h"
)

add_definitions(-DUSE_LGPIO_LIB)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC 
    "lib/Config"
    "lib/Driver"
    "lib/EPD"
    "lib/Fonts"
    "lib/GUI"
)