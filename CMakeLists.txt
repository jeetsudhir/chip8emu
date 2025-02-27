cmake_minimum_required(VERSION 3.28)
project(chip8Emulator)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Search for SDL2 package
find_package(SDL2 REQUIRED)

if (SDL2_FOUND)
    message(STATUS "SDL2 found: ${SDL2_VERSION}")
    message(STATUS "SDL2 include directories: ${SDL2_INCLUDE_DIRS}")
    message(STATUS "SDL2 libraries: ${SDL2_LIBRARIES}")
else()
    message(FATAL_ERROR "SDL2 not found! Please install SDL2 or set SDL2_DIR to the location of SDL2Config.cmake.")
endif()

# Include SDL2 headers
include_directories(${SDL2_INCLUDE_DIRS})

# Add the executable and source files
add_executable(chip8Emulator main.cpp)

# Add additional sources to the executable
target_sources(chip8Emulator
        PRIVATE
        chip8core.cpp
        sound.cpp
        screen.cpp
        PUBLIC
        chip8core.h
        fontset.h
        sound.h
        screen.h
)

# Link SDL2 libraries
target_link_libraries(chip8Emulator PRIVATE SDL2::SDL2)


