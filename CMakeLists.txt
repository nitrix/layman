CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(layman C)

SET(C_STANDARD 11)

ADD_SUBDIRECTORY(lib/glad)
ADD_SUBDIRECTORY(lib/glfw)
ADD_SUBDIRECTORY(lib/gltf)

ADD_LIBRARY(
    layman
    src/application.c
    src/entity.c
    src/material.c
    src/matrix.c
    src/mesh.c
    src/model.c
    src/renderer.c
    src/scene.c
    src/shader.c
    src/texture.c
    src/vector.c
    src/window.c
)

TARGET_INCLUDE_DIRECTORIES(layman PUBLIC include lib/stb)
TARGET_LINK_LIBRARIES(layman PRIVATE glad glfw gltf)
TARGET_COMPILE_OPTIONS(layman PRIVATE -std=c11 -Wall -Wextra -static)
TARGET_COMPILE_DEFINITIONS(layman PRIVATE "$<$<CONFIG:DEBUG>:DEBUG>")
TARGET_COMPILE_OPTIONS(layman PRIVATE "$<$<CONFIG:DEBUG>:-O0;-g>")
# TARGET_COMPILE_OPTIONS(layman PRIVATE "$<$<CONFIG:DEBUG>:-fsanitize=address,undefined>")
# TARGET_COMPILE_OPTIONS(layman PRIVATE "$<$<CONFIG:DEBUG>:-fsanitize=undefined;-fsanitize-undefined-trap-on-error>")
TARGET_COMPILE_OPTIONS(layman PRIVATE "$<$<CONFIG:RELEASE>:-O3>")

# This tells GLFW to not include OpenGL, we use Glad for that.
TARGET_COMPILE_DEFINITIONS(layman PRIVATE -DGLFW_INCLUDE_NONE)

# FIXME: Temporary test executable.
ADD_EXECUTABLE(example src/example.c)
TARGET_LINK_LIBRARIES(example PRIVATE layman)