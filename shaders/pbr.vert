#version 330 core

in vec3 position;
//in vec2 uv;
//in vec3 normal;
//in vec4 tangent;

out vec3 WorldPosition;

void main() {
	gl_Position = vec4(position, 1.0);
	WorldPosition = gl_Position.xyz;
}