#version 400 core

in vec3 position;
in vec2 texture_coords;

// out vec3 color;
out vec2 pass_texture_coords;

uniform mat4 transform;

void main(void) {
    // gl_Position = vec4(position.x, position.y, position.z, 1.0);
    // color = vec3(position.x + 0.5, 1.0, position.y + 0.5);
    // pass_texture_coords = vec2(1.0, 1.0);
    pass_texture_coords = texture_coords;

    gl_Position = transform * vec4(position.x, position.y, position.z, 1.0);
}