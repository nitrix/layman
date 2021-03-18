CMAKE_MINIMUM_REQUIRED(VERSION 3.9)
PROJECT(layman)

SET(C_STANDARD 11)

# ======================================================================================================================
#                                                    INCBIN
# ======================================================================================================================

# TODO: Provide them a CMakeLists.txt?
ADD_LIBRARY(incbin STATIC lib/incbin/incbin.c)
TARGET_INCLUDE_DIRECTORIES(incbin PUBLIC lib/incbin)

# ======================================================================================================================
#                                                    CIMGUI
# ======================================================================================================================

SET(IMGUI_STATIC true)
FILE(GLOB 
    cimgui_files 
	"lib/cimgui/*.cpp" 
	"lib/cimgui/imgui/*.cpp"
	"lib/cimgui/imgui/backends/imgui_impl_glfw.cpp"
	"lib/cimgui/imgui/backends/imgui_impl_opengl3.cpp"
)

ADD_LIBRARY(cimgui ${cimgui_files})

TARGET_COMPILE_DEFINITIONS(cimgui PRIVATE IMGUI_IMPL_API=extern\ \"C\" IMGUI_IMPL_OPENGL_LOADER_GLAD)
TARGET_INCLUDE_DIRECTORIES(cimgui PRIVATE lib/cimgui/imgui)
TARGET_LINK_LIBRARIES(cimgui PRIVATE stdc++)
TARGET_INCLUDE_DIRECTORIES(cimgui PUBLIC lib/cimgui/generator/output)
TARGET_INCLUDE_DIRECTORIES(cimgui PUBLIC lib/cimgui)
TARGET_LINK_LIBRARIES(cimgui PRIVATE glfw glad)
TARGET_COMPILE_DEFINITIONS(cimgui INTERFACE CIMGUI_DEFINE_ENUMS_AND_STRUCTS)

# Work-around for regression in cimgui introduced by imgui 1.82. See https://github.com/cimgui/cimgui/issues/184
IF(WIN32)
    TARGET_LINK_LIBRARIES(cimgui PRIVATE imm32)
ENDIF()

# ======================================================================================================================
#                                                    LAYMAN
# ======================================================================================================================

ADD_SUBDIRECTORY(lib/cglm)
ADD_SUBDIRECTORY(lib/glad)
ADD_SUBDIRECTORY(lib/glfw)
ADD_SUBDIRECTORY(lib/gltf)
ADD_SUBDIRECTORY(lib/stb)

ADD_LIBRARY(
    layman STATIC
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
    ${layman_resources}
)

# Private includes must happen before public includes to have higher priority.
TARGET_INCLUDE_DIRECTORIES(layman PRIVATE private)
TARGET_INCLUDE_DIRECTORIES(layman PUBLIC public)

TARGET_LINK_LIBRARIES(layman PRIVATE cglm glad glfw gltf stb_image cimgui incbin)
TARGET_COMPILE_OPTIONS(layman PRIVATE -std=c11 -Wall -Wextra -static)
TARGET_COMPILE_DEFINITIONS(layman PRIVATE "$<$<CONFIG:DEBUG>:LAYGL_DEBUG>")
TARGET_COMPILE_OPTIONS(layman PRIVATE "$<$<CONFIG:DEBUG>:-O0;-g;-ggdb>")
TARGET_COMPILE_OPTIONS(layman PRIVATE "$<$<CONFIG:RELEASE>:-O3>")

# This tells GLFW to not include OpenGL, we use Glad for that.
TARGET_COMPILE_DEFINITIONS(layman PRIVATE GLFW_INCLUDE_NONE)

# No prefix and snake case for incbin.
TARGET_COMPILE_DEFINITIONS(layman PRIVATE INCBIN_PREFIX=\ )
TARGET_COMPILE_DEFINITIONS(layman PRIVATE INCBIN_STYLE=INCBIN_STYLE_SNAKE)