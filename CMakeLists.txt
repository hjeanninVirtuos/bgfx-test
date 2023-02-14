cmake_minimum_required(VERSION 3.0)

project(bgfx-test)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "Build the GLFW test programs"    FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "Build the GLFW documentation"    FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "Generate installation target"    FORCE)

set(BGFX_BUILD_TOOLS    ON  CACHE BOOL "Build bgfx tools."                      FORCE)
set(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "Build bgfx examples."                   FORCE)
set(BGFX_INSTALL        OFF CACHE BOOL "Create installation target."            FORCE)
set(BGFX_CUSTOM_TARGETS OFF CACHE BOOL "Include convenience custom targets."    FORCE)

add_subdirectory(modules/bgfx)
add_subdirectory(modules/glfw)

add_executable(${CMAKE_PROJECT_NAME} source/main.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME}
    glfw
    bx
    bimg
    bgfx
)

#-------------------------------------------------------------------------------
# Add shaders
#-------------------------------------------------------------------------------
include(vts_bgfx.cmake)

vts_bgfx_compile_shader_to_header(
    TYPE        VERTEX
    SHADERS     ${CMAKE_CURRENT_SOURCE_DIR}/shaders/dg_dmapack_vs.sc
    VARYING_DEF ${CMAKE_CURRENT_SOURCE_DIR}/shaders/dg_dmapack_def.sc
    OUTPUT_DIR  ${CMAKE_BINARY_DIR}/include/generated/shaders
)

vts_bgfx_compile_shader_to_header(
    TYPE        FRAGMENT
    SHADERS     ${CMAKE_CURRENT_SOURCE_DIR}/shaders/dg_dmapack_fs.sc
    VARYING_DEF ${CMAKE_CURRENT_SOURCE_DIR}/shaders/dg_dmapack_def.sc
    OUTPUT_DIR  ${CMAKE_BINARY_DIR}/include/generated/shaders
)

set(SHADER_FILES
    shaders/dg_dmapack_vs.sc
    shaders/dg_dmapack_fs.sc
    shaders/dg_dmapack_def.sc
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Shader Files" FILES ${SHADER_FILES})

target_sources(bgfx-test PRIVATE ${SHADER_FILES})

target_include_directories(bgfx-test PRIVATE ${CMAKE_BINARY_DIR}/include/generated/shaders)

