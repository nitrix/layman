CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(learngl)

# GLEW
ADD_SUBDIRECTORY(lib/glew-cmake)
INCLUDE_DIRECTORIES(lib/glew-cmake/include)

# GLFW
ADD_SUBDIRECTORY(lib/glfw)
INCLUDE_DIRECTORIES(lib/glfw/include)

# Learn GL
ADD_EXECUTABLE(learngl
    src/main.c
    src/renderer.c
    src/window.c
)
TARGET_LINK_LIBRARIES(learngl glew32 glfw3 opengl32)
TARGET_INCLUDE_DIRECTORIES(learngl PRIVATE include)