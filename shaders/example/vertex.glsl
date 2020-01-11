#version 400 core

in vec3 position;
in vec2 texture_coords;

out vec2 pass_texture_coords;

uniform mat4 transformation;
uniform mat4 projection;

void main(void) {
    pass_texture_coords = texture_coords;
    gl_Position = projection * transformation * vec4(position.x, position.y, position.z, 1.0);
}