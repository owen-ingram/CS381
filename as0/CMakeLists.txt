cmake_minimum_required(VERSION 3.18)
project(as0 LANGUAGES CXX)

add_subdirectory(raylib-cpp)

file(GLOB sources "src/*.cpp" "src/*.c")
add_executable(as0 ${sources})
target_link_libraries(as0 PUBLIC raylib raylib_cpp)
set_property(TARGET as0 PROPERTY CXX_STANDARD 20)