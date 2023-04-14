cmake_minimum_required(VERSION 3.25)
project(sysY)

set(CMAKE_CXX_STANDARD 14)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(sysY ${SOURCES})

target_include_directories(sysY
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)
