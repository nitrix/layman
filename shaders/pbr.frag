#version 330 core

in vec3 WorldPosition;
in vec2 UV;

out vec4 out_color;

uniform sampler2D albedo_map;

void main() {
	out_color = texture(albedo_map, UV);
}