cmake_minimum_required(VERSION 3.0)

project(bgfx-test)

add_subdirectory(modules/bgfx)
add_subdirectory(modules/glfw)

add_executable(${CMAKE_PROJECT_NAME} source/main.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME}
    glfw
    bx
    bimg
    bgfx
)
