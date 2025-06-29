cmake_minimum_required(VERSION 3.16)
project(RayTracerImGui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set source files
file(GLOB_RECURSE SOURCES
    src/*.cpp
)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(vendors/glfw)

# GLAD
add_library(glad
    vendors/glad/src/glad.c
)
target_include_directories(glad PUBLIC vendors/glad/include)

# ImGui
file(GLOB IMGUI_SRC
    vendors/imgui/imgui.cpp
    vendors/imgui/imgui_draw.cpp
    vendors/imgui/imgui_tables.cpp
    vendors/imgui/imgui_widgets.cpp
    vendors/imgui/backends/imgui_impl_glfw.cpp
    vendors/imgui/backends/imgui_impl_opengl3.cpp
)
add_library(imgui ${IMGUI_SRC})
target_include_directories(imgui PUBLIC
    vendors/imgui
    vendors/imgui/backends
    vendors/glfw/include
)

# Executable
add_executable(raytracer ${SOURCES})

# Includes
target_include_directories(raytracer PRIVATE
    src
    vendors/glad/include
    vendors/imgui
    vendors/imgui/backends
    vendors/glfw/include
)

# Link libraries
target_link_libraries(raytracer PRIVATE glfw glad imgui)

# Windows-specific linking
if(WIN32)
    target_link_libraries(raytracer PRIVATE opengl32)
endif()
