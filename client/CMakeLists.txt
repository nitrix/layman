ADD_EXECUTABLE(
    client
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
    src/ui.c
    src/window.c
    src/main.c
)

TARGET_COMPILE_OPTIONS(client PRIVATE -std=c11 -Wall -Wextra -static)
TARGET_LINK_LIBRARIES(client PRIVATE cglm glad glfw gltf stb_image cimgui incbin)
TARGET_INCLUDE_DIRECTORIES(client PRIVATE include)

# This tells GLFW to not include OpenGL, we use Glad for that.
TARGET_COMPILE_DEFINITIONS(client PRIVATE GLFW_INCLUDE_NONE)

# No prefix and snake case for incbin.
TARGET_COMPILE_DEFINITIONS(client PRIVATE INCBIN_PREFIX=\ )
TARGET_COMPILE_DEFINITIONS(client PRIVATE INCBIN_STYLE=INCBIN_STYLE_SNAKE)

TARGET_COMPILE_DEFINITIONS(client PRIVATE "$<$<CONFIG:DEBUG>:DEBUG>")
TARGET_COMPILE_OPTIONS(client PRIVATE "$<$<CONFIG:DEBUG>:-O0;-g;-ggdb>")
TARGET_COMPILE_OPTIONS(client PRIVATE "$<$<CONFIG:RELEASE>:-O3>")