#version 400 core

// in vec3 color;
in vec2 pass_texture_coords;

out vec4 out_Color;

uniform sampler2D texture_sampler;

void main(void) {
    // out_Color = vec4(color, 1.0);
    out_Color = texture(texture_sampler, pass_texture_coords);
    // out_Color = vec4(pass_texture_coords, 1.0, 1.0);
}