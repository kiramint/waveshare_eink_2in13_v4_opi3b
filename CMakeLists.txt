cmake_minimum_required(VERSION 3.21.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)

project(WaveshareDrivers)

file(GLOB SOURCES 
    "lib/Config/DEV_Config.c"
    "lib/Driver/*.c"
    "lib/EPD/*.c"
    "lib/Fonts/*.c"
    "lib/GUI/*.c"
)

file(GLOB HEADERS
    "lib/Config/Debug.h"
    "lib/Config/DEV_Config.h"
    "lib/Driver/*.h"
    "lib/EPD/*.h"
    "lib/Fonts/*.h"
    "lib/GUI/*.h"
)

file(COPY ${HEADERS} DESTINATION ${CMAKE_BINARY_DIR}/include)

add_definitions(-DUSE_LGPIO_LIB)

find_library(LGPIO_LIB lgpio REQUIRED)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC ${LGPIO_LIB})

target_include_directories(${PROJECT_NAME} PUBLIC 
    "lib/Config"
    "lib/Driver"
    "lib/EPD"
    "lib/Fonts"
    "lib/GUI"
)