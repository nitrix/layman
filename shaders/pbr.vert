#version 330 core

in vec3 position;
in vec2 uv;
in vec3 normal;
in vec4 tangent;

out vec2 UV;
out vec3 WorldPosition;
out mat3 TBN;

uniform mat4 model_transform;
uniform mat4 normal_transform;
uniform mat4 view_transform;
uniform mat4 projection_transform;

void main() {
    gl_Position = projection_transform * view_transform * model_transform * vec4(position, 1.0);
    WorldPosition = vec3(model_transform * vec4(position, 1.0));
    UV = uv;

    // Generate TBN (tangent space to world space conversion matrix).
    vec3 T = normalize(mat3(normal_transform) * tangent.xyz);
    vec3 N = normalize(mat3(normal_transform) * normal);
    vec3 B = normalize(cross(T, N));
    TBN = mat3(T, B, N);
}