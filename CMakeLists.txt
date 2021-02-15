CMAKE_MINIMUM_REQUIRED(VERSION 3.9)
PROJECT(layman C)

SET(C_STANDARD 11)

# Link-Time Optimization (LTO)
# CMAKE_POLICY(SET CMP0069 NEW)
# SET(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

ADD_SUBDIRECTORY(lib/cglm)
ADD_SUBDIRECTORY(lib/glad)
ADD_SUBDIRECTORY(lib/glfw)
ADD_SUBDIRECTORY(lib/gltf)
ADD_SUBDIRECTORY(lib/stb)

ADD_LIBRARY(
    layman
    src/camera.c
    src/entity.c
    src/environment.c
    src/framebuffer.c
    src/light.c
    src/material.c
    src/mesh.c
    src/model.c
    src/renderer.c
    src/scene.c
    src/shader.c
    src/texture.c
    src/window.c
)

# Private includes must happen before public includes to have higher priority.
TARGET_INCLUDE_DIRECTORIES(layman PRIVATE private)
TARGET_INCLUDE_DIRECTORIES(layman PUBLIC public)

TARGET_LINK_LIBRARIES(layman PRIVATE cglm glad glfw gltf stb_image)
TARGET_COMPILE_OPTIONS(layman PRIVATE -std=c11 -Wall -Wextra -static)
TARGET_COMPILE_DEFINITIONS(layman PRIVATE "$<$<CONFIG:DEBUG>:LAYGL_DEBUG>")
TARGET_COMPILE_OPTIONS(layman PRIVATE "$<$<CONFIG:DEBUG>:-O0;-g;-ggdb>")
TARGET_COMPILE_OPTIONS(layman PRIVATE "$<$<CONFIG:RELEASE>:-O3>")

# This tells GLFW to not include OpenGL, we use Glad for that.
TARGET_COMPILE_DEFINITIONS(layman PRIVATE -DGLFW_INCLUDE_NONE)

# FIXME: Temporary test executable.
ADD_EXECUTABLE(example example/main.c)
TARGET_LINK_LIBRARIES(example PUBLIC layman)
TARGET_COMPILE_OPTIONS(example PUBLIC "$<$<CONFIG:DEBUG>:-O0;-g;-ggdb>")
TARGET_COMPILE_OPTIONS(example PUBLIC "$<$<CONFIG:RELEASE>:-O3>")