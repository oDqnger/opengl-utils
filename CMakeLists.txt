cmake_minimum_required(VERSION 3.10)
project(OpenGLProject C)
set(CMAKE_C_STANDARD 11)
# Include header files
include_directories(include)
# All source files
set(SOURCES
    src/main.c
    src/utils/glad.c
    src/utils/shader.c
    src/utils/stb_image.c
    src/utils/camera.c
    src/utils/textures.c
    src/utils/mesh.c
    src/utils/model.c
    src/utils/lighting.c
    src/utils/block.c
    src/utils/perlin.c
    src/utils/nuklear.c
)
# Create executable
add_executable(main ${SOURCES})
# Find libraries
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)
# Link libraries
target_link_libraries(main
    glfw
    OpenGL::GL
    assimp
    m
    dl
)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
