CMAKE_MINIMUM_REQUIRED(VERSION 3.9)
PROJECT(layman)

SET(C_STANDARD 11)

ADD_SUBDIRECTORY(cmake/cimgui)
ADD_SUBDIRECTORY(cmake/incbin)

ADD_SUBDIRECTORY(lib/cglm)
ADD_SUBDIRECTORY(lib/glad)
ADD_SUBDIRECTORY(lib/glfw)
ADD_SUBDIRECTORY(lib/gltf)
ADD_SUBDIRECTORY(lib/toolkit)
ADD_SUBDIRECTORY(lib/stb)

ADD_EXECUTABLE(
    layman
    src/camera.c
    src/client.c
    src/entity.c
    src/environment.c
    src/framebuffer.c
    src/gizmo.c
    src/light.c
    src/main.c
    src/material.c
    src/mesh.c
    src/model.c
    src/modelmanager.c
    src/renderer.c
    src/scene.c
    src/server.c
    src/shader.c
    src/texture.c
    src/ui.c
    src/utils.c
    src/window.c
)

TARGET_COMPILE_OPTIONS(layman PRIVATE -std=c11 -Wall -Wextra -static)
TARGET_LINK_LIBRARIES(layman PRIVATE cglm glad glfw gltf stb_image cimgui incbin toolkit)
TARGET_INCLUDE_DIRECTORIES(layman PRIVATE include)

# This tells GLFW to not include OpenGL, we use Glad for that.
TARGET_COMPILE_DEFINITIONS(layman PRIVATE GLFW_INCLUDE_NONE)

# No prefix and snake case for incbin.
TARGET_COMPILE_DEFINITIONS(layman PRIVATE INCBIN_PREFIX=\ )
TARGET_COMPILE_DEFINITIONS(layman PRIVATE INCBIN_STYLE=INCBIN_STYLE_SNAKE)

TARGET_COMPILE_DEFINITIONS(layman PRIVATE "$<$<CONFIG:DEBUG>:DEBUG>")
TARGET_COMPILE_OPTIONS(layman PRIVATE "$<$<CONFIG:DEBUG>:-O0;-g;-ggdb>")
TARGET_COMPILE_OPTIONS(layman PRIVATE "$<$<CONFIG:RELEASE>:-O3>")

# Useful sometimes for debugging.
# TARGET_COMPILE_OPTIONS(layman PRIVATE -fsanitize=undefined -fsanitize-undefined-trap-on-error)
# TARGET_COMPILE_OPTIONS(layman PRIVATE -fsanitize=address)
# TARGET_LINK_OPTIONS(layman PRIVATE -fsanitize=undefined)
# TARGET_LINK_OPTIONS(layman PRIVATE -fsanitize=address)