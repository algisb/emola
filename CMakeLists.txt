cmake_minimum_required (VERSION 3.1.0)

project (emola)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "./bin")
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "./bin")
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "./bin")

include_directories ("./src")
file(GLOB EMOLA_SRC "./src/*.c")
add_executable(emola ${EMOLA_SRC})
