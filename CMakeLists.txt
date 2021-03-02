CMAKE_MINIMUM_REQUIRED(VERSION 3.9)
PROJECT(layman C)

SET(C_STANDARD 11)

# Consider using https://github.com/graphitemaster/incbin/ to embed files instead of this linker monstruosity.
SET(RESOURCES "")
FUNCTION(ADD_RESOURCE input)
    STRING(MAKE_C_IDENTIFIER ${input} input_identifier)
    SET(output "${CMAKE_CURRENT_BINARY_DIR}/${input_identifier}.o")

    ADD_CUSTOM_COMMAND(
        OUTPUT ${output}
        COMMAND ${CMAKE_LINKER} --relocatable --format binary --output ${output} ${input}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${input}"
    )

    SET(RESOURCES ${RESOURCES} ${output} PARENT_SCOPE)
ENDFUNCTION()

ADD_SUBDIRECTORY(lib/cglm)
ADD_SUBDIRECTORY(lib/glad)
ADD_SUBDIRECTORY(lib/glfw)
ADD_SUBDIRECTORY(lib/gltf)
ADD_SUBDIRECTORY(lib/stb)

ADD_RESOURCE("shaders/equirect2cube/main.vert")
ADD_RESOURCE("shaders/equirect2cube/main.frag")
ADD_RESOURCE("shaders/iblsampler/main.vert")
ADD_RESOURCE("shaders/iblsampler/main.frag")
ADD_RESOURCE("shaders/skybox/main.vert")
ADD_RESOURCE("shaders/skybox/main.frag")
ADD_RESOURCE("shaders/pbr/main.vert")
ADD_RESOURCE("shaders/pbr/main.frag")

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
    ${RESOURCES}
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
TARGET_COMPILE_DEFINITIONS(layman PRIVATE GLFW_INCLUDE_NONE)