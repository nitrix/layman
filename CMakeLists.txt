CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(learngl)

# Learn GL
ADD_EXECUTABLE(learngl
    src/main.c
    src/renderer.c
    src/window.c
)
TARGET_INCLUDE_DIRECTORIES(learngl PRIVATE include)

# OpenGL
TARGET_LINK_LIBRARIES(learngl PRIVATE opengl32)

# GLEW
ADD_SUBDIRECTORY(lib/glew-cmake)
TARGET_INCLUDE_DIRECTORIES(learngl PRIVATE lib/glew-cmake/include)
TARGET_LINK_LIBRARIES(learngl PRIVATE libglew_static)

# GLFW
ADD_SUBDIRECTORY(lib/glfw)
TARGET_INCLUDE_DIRECTORIES(learngl PRIVATE lib/glfw/include)
TARGET_LINK_LIBRARIES(learngl PRIVATE glfw)

# Toolkit
ADD_SUBDIRECTORY(lib/toolkit)
TARGET_INCLUDE_DIRECTORIES(learngl PRIVATE lib/toolkit/include)
TARGET_LINK_LIBRARIES(learngl PRIVATE toolkit)
